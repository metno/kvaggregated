/*
 Kvalobs - Free Quality Control Software for Meteorological Observations

 $Id: KvalobsProxy.h,v 1.1.2.7 2007/09/27 09:02:16 paule Exp $

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
#ifndef __kvservice__proxy__KvalobsProxy_h__
#define __kvservice__proxy__KvalobsProxy_h__

#include "DataAccess.h"
#include "CachedDataAccess.h"
#include "KvalobsDataAccess.h"
#include "CallbackCollection.h"
#include "ReadWriteLock.h"
#include <boost/utility.hpp>
#include <set>
#include <dnmithread/CommandQue.h>
#include <decodeutility/kvDataFormatter.h>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include "metrics.h"

namespace kvservice
{
namespace proxy
{
class KvalobsProxy: public DataAccess
{
public:
	KvalobsProxy(const std::string & proxyDatabaseName,
			bool repopulate = false);
	virtual ~KvalobsProxy();

	virtual void getData(Metrics &m, KvDataList &data, int station, const boost::posix_time::ptime &from,
			const boost::posix_time::ptime &to, int paramid, int type, int sensor,
			int lvl) const;

	virtual CKvalObs::CDataSource::Result_var sendData(Metrics &m,const KvDataList &data);

	virtual void cacheData(Metrics &m, const KvDataList &data);

	/**
	 * Add parameter to store in cache database.
	 *
	 * @warning Not thread-safe!
	 */
	virtual void addInteresting(Metrics &m, int param)
	{
		cache_.addInteresting(m ,param);
		kvalobs_.addInteresting(m, param);
		interestingParameters_.insert(param);
	}

	virtual float getStationMetadata(Metrics &m,const std::string & metadataName, const kvalobs::kvData & validFor) const
	{
		return kvalobs_.getStationMetadata(m, metadataName, validFor);
	}

	// Operations on proxy:
	void db_clear();
	void db_cleanup();
	void db_populate(Metrics &m, int hours = 24);
	void db_populate(Metrics &m,const boost::posix_time::ptime & from, const boost::posix_time::ptime & to);
	void db_repopulate(Metrics &m,int hours = 24)
	{
		db_clear();
		db_populate(m, hours);
	}

	void setOldestInProxy(const boost::posix_time::ptime & newTime);

	KvalobsDataAccess & directKvalobsAccess() { return kvalobs_; }
	const KvalobsDataAccess & directKvalobsAccess() const { return kvalobs_; }

private:
	/// True if the given data's values (original or corrected) are different from kvalobs'
	bool updatesKvalobs_(Metrics &m,const kvalobs::kvData & data) const;

	std::set<int> interestingParameters_;

	// We only adapt and send a single set of data to kvalobs at a time
	boost::mutex sendDataMutex_;

	/**
	 * Protect oldestInProxy variable. We assume that modifications to
	 * oldestInProxy are rare and have no requirement for fast action.
	 * Therefore we use an unfair lock, which causes starvation.
	 */
	mutable RWMutex timeMutex_;
	boost::posix_time::ptime oldestInProxy;

	CachedDataAccess cache_;
	KvalobsDataAccess kvalobs_;

	class Cleaner;
	Cleaner * cleaner_;
	boost::thread * cleanerThread_;
};
}
}

#endif // __kvservice__proxy__KvalobsProxy_h__
