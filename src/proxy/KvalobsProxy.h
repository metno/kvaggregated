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

	virtual void getData(KvDataList &data, int station, const boost::posix_time::ptime &from,
			const boost::posix_time::ptime &to, int paramid, int type, int sensor,
			int lvl) const;

	virtual CKvalObs::CDataSource::Result_var sendData(const KvDataList &data);

	virtual void cacheData(const KvDataList &data);

	/**
	 * Add parameter to store in cache database.
	 *
	 * @warning Not thread-safe!
	 */
	virtual void addInteresting(int param)
	{
		cache_.addInteresting(param);
		kvalobs_.addInteresting(param);
		interestingParameters_.insert(param);
	}

	virtual float getStationMetadata(const std::string & metadataName, const kvalobs::kvData & validFor) const
	{
		return kvalobs_.getStationMetadata(metadataName, validFor);
	}

	// Operations on proxy:
	void db_clear();
	void db_cleanup();
	void db_populate(int hours = 24);
	void db_populate(const boost::posix_time::ptime & from, const boost::posix_time::ptime & to);
	void db_repopulate(int hours = 24)
	{
		db_clear();
		db_populate(hours);
	}

	void setOldestInProxy(const boost::posix_time::ptime & newTime);

	KvalobsDataAccess & directKvalobsAccess() { return kvalobs_; }
	const KvalobsDataAccess & directKvalobsAccess() const { return kvalobs_; }

private:
	/// True if the given data's values (original or corrected) are different from kvalobs'
	bool updatesKvalobs_(const kvalobs::kvData & data) const;

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
