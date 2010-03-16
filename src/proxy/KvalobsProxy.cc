/*
  Kvalobs - Free Quality Control Software for Meteorological Observations 

  $Id: KvalobsProxy.cc,v 1.2.2.5 2007/09/27 09:02:16 paule Exp $                                                       

  Copyright (C) 2007 met.no

  Contact information:
  Norwegian Meteorological Institute
  Box 43 Blindern
  0313 OSLO
  NORWAY
  email: kvalobs-dev@met.no

  This file is part of KVALOBS

  KVALOBS is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as 
  published by the Free Software Foundation; either version 2 
  of the License, or (at your option) any later version.
  
  KVALOBS is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.
  
  You should have received a copy of the GNU General Public License along 
  with KVALOBS; if not, write to the Free Software Foundation Inc., 
  51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include "KvalobsProxy.h"
#include "Callback.h"
#include "KvDataReceiver.h"
#include "IncomingHandler.h"
#include "ProxyDatabaseConnection.h"
#include <kvalobs/kvDataOperations.h>
#include <kvcpp/KvApp.h>
#include <kvcpp/kvevents.h>
#include <milog/milog.h>
#include <sstream>
#include <utility>
#include <cmath>
#include <algorithm>
#include <functional>
#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/scoped_ptr.hpp>

//#define NDEBUG
#include <cassert>

using namespace std;
using namespace kvalobs;
using namespace miutil;
using namespace milog;
using namespace dnmi::db;
using namespace dnmi::thread;

namespace kvservice
{
namespace proxy
{
	class KvalobsProxy::Cleaner
	{
	public:
		Cleaner(KvalobsProxy & proxy) :
			proxy_(proxy), d(new SharedData)
		{}

		void operator () ()
		{
			d->stop_ = false;

			boost::xtime time;
			boost::xtime_get(& time, boost::TIME_UTC);

			miutil::miClock startTime = miutil::miClock::oclock();
			miutil::miClock cleanTime(2, 20 ,0);

			int timeLeft = miutil::miClock::secDiff(cleanTime, startTime);
			if ( timeLeft <= 0 )
				timeLeft += 60 * 60 * 24;

			time.nsec = 0;
			time.sec += timeLeft;
			while ( not d->stop_ )
			{
				boost::mutex::scoped_lock l(d->mutex_);
				d->condition.timed_wait(l, time);
				time.sec += 60 * 60 * 24;

				if ( d->stop_ )
					break;
				proxy_.db_cleanup();
			}
		}

		void stop()
		{
			boost::mutex::scoped_lock l(d->mutex_);
			d->stop_ = true;
			d->condition.notify_all();
		}

	private:
		struct SharedData
		{
			boost::condition condition;
	        boost::mutex mutex_;
			bool stop_;
		};
		boost::shared_ptr<SharedData> d;
		KvalobsProxy & proxy_;
	};


    KvalobsProxy::KvalobsProxy( const std::string & proxyDatabaseName, CallbackCollection & callbacks, bool repopulate )
        : cache_( proxyDatabaseName )
        , oldestInProxy( miTime::nowTime() )
    {
      if ( ! KvApp::kvApp )
      {
        const char * msg = "Cannot find an instance of KvApp!";
        LOGFATAL( msg );
        throw runtime_error( msg );
      }

      // Make reasonably sure the proxy is correct.
      oldestInProxy.addHour( 2 );

      LogContext context( "KvalobsProxy::KvalobsProxy" );

      if ( repopulate )
        db_repopulate();

      // yes, this works:
      cleaner_ = new Cleaner(* this);
      cleanerThread_ = new boost::thread(* cleaner_);
    }

    KvalobsProxy::~KvalobsProxy( )
    {
    	cleaner_->stop();
    	cleanerThread_->join();
    	delete cleaner_;
    	delete cleanerThread_;
    }

    void KvalobsProxy::db_clear()
    {
    	{
			ScopedWriteLock lock(timeMutex_);
			oldestInProxy = miTime::nowTime();
			oldestInProxy.addHour();
    	}
    	cache_.clear();
    }

    void KvalobsProxy::db_populate(int hours)
	{
    	KvDataList data;
		miTime to = miTime::nowTime();
		miTime from = to;
		from.addHour(-hours);

		kvalobs_.getAllData(data, from, to);

		cache_.sendData(data);

		ScopedWriteLock lock(timeMutex_);
		if (oldestInProxy.undef())
			oldestInProxy = from;
		else
			oldestInProxy = min(oldestInProxy, from);
		LOGINFO("Got data from source database. Current oldest observation:"
				<< oldestInProxy << ".");
	}

    void KvalobsProxy::db_cleanup()
    {
    	LOGINFO( "KvalobsProxy::db_cleanup" );
    	miTime t = miTime::nowTime();
    	t.addDay(- 35);

    	{
			ScopedWriteLock lock(timeMutex_);
			if (oldestInProxy.undef())
				oldestInProxy = t;
			else
				oldestInProxy = max(oldestInProxy, t);
    	}

    	cache_.deleteOldData(t);
    	LOGINFO("KvalobsProxy::db_cleanup: Done");
    }

    namespace
    {
    bool differentCorrected(const kvData & a, const kvData & b)
	{
		bool va = valid(a);
		bool vb = valid(b);
		if (va != vb)
			return true;
		if (va and vb)
		{
			float diff = abs(a.corrected() - b.corrected());
			return diff > 0.04999;
		}
		return false;
	}

    bool differentOriginal(const kvData & a, const kvData & b)
	{
		bool va = not original_missing(a);
		bool vb = not original_missing(b);
		if (va != vb)
			return true;
		if (va and vb)
		{
			float diff = abs(a.original() - b.original());
			return diff > 0.04999;
		}
		return false;
	}


    bool different_( const kvData & a, const kvData & b )
    {
    	return differentCorrected(a,b) or differentOriginal(a,b);
    }
    }

    CKvalObs::CDataSource::Result_var KvalobsProxy::sendData(const KvDataList &data)
	{
		LogContext context("KvalobsProxy::sendData");
		KvDataList l;

		boost::mutex::scoped_lock lock(sendDataMutex_);

		for (CIKvDataList it = data.begin(); it != data.end(); it++)
		{
			if ( interestingParameters_.find(it->paramID()) == interestingParameters_.end() )
				continue;

//			adaptDataToKvalobs_(l, * it);
			if ( updatesKvalobs_(* it) )
				l.push_back(* it);
		}
		if (l.empty())
		{
			LOGDEBUG( "No new data to send (kvalobs had all data from before)" );
			CKvalObs::CDataSource::Result_var res(new CKvalObs::CDataSource::Result);
			res->res = CKvalObs::CDataSource::OK;
			res->message = "No data";
			return res;
		}

		CKvalObs::CDataSource::Result_var res = kvalobs_.sendData(l);

		if (res->res == CKvalObs::CDataSource::OK)
			cacheData(l);
		else
			LOGERROR("Error when sending data to kvalobs: " << res->message);

		return res;
	}

    void KvalobsProxy::getData( KvDataList &data, int station,
                                const miutil::miTime &from, const miutil::miTime &to,
                                int paramid, int type, int sensor, int lvl ) const
    {
      //LogContext context( "KvalobsProxy::getData" );

      // Fetch data from kvalobs, if neccessary
    	ScopedReadLock lock(timeMutex_);
      if ( oldestInProxy.undef() or from <= oldestInProxy )
      {
        miTime k_from = from;
        miTime k_to = oldestInProxy.undef() ? to : min( to, oldestInProxy );
        //LOGDEBUG( "Fetching times " << from << " - " << k_to << " from kvalobs" );
        kvalobs_.getData( data, station, k_from, k_to, paramid, type, sensor, lvl );
        LOGDEBUG( "Data from kvalobs :\n" << decodeutility::kvdataformatter::createString( data ) );
      }

      // Fetch data from proxy, if neccessary
      if ( oldestInProxy.undef() or to > oldestInProxy )
      {
        miTime p_from = oldestInProxy.undef() ? from : max( from, oldestInProxy );
        //LOGDEBUG( "Fetching times " << p_from << " - " << to << " from proxy" );
        KvDataList proxyData;
        cache_.getData( proxyData, station, p_from, to, paramid, type, sensor, lvl );
        for ( KvDataList::const_iterator it = proxyData.begin(); it != proxyData.end(); ++ it )
        {
        	KvDataList::const_iterator find = find_if(data.begin(), data.end(), 
        			bind1st(kvalobs::compare::same_kvData(),*it)); 
        	if ( find == data.end() )
        		data.push_back(* it);
        }
      }
    }

    void KvalobsProxy::cacheData(const KvDataList & data)
    {
    	KvDataList toSave;
    	for ( CIKvDataList it = data.begin(); it != data.end(); ++ it )
    		if ( interestingParameters_.find(it->paramID()) != interestingParameters_.end() )
    			toSave.push_back(* it);

    	if ( not toSave.empty() )
    		cache_.sendData(toSave);
    }

    void KvalobsProxy::setOldestInProxy( const miutil::miTime & newTime )
    {
    	ScopedWriteLock lock(timeMutex_);
    	oldestInProxy = newTime;
    }


bool KvalobsProxy::updatesKvalobs_(const kvalobs::kvData & data) const
{
	KvDataList dl;
	getData(dl, data.stationID(), data.obstime(), data.obstime(),
			data.paramID(), data.typeID(), data.sensor(), data.level());

	if (dl.empty())
		return true;
	return different_(dl.front(), data);
}

}
}
