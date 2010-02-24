/*
 Kvalobs - Free Quality Control Software for Meteorological Observations 

 Copyright (C) 2010 met.no

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

#include "CachedDataAccess.h"
#include "ProxyDatabaseConnection.h"
#include <milog/milog.h>
#include <sstream>
#include <memory>

using namespace std;
using namespace milog;
using namespace dnmi::db;

namespace kvservice
{

CachedDataAccess::CachedDataAccess(const std::string & proxyDatabaseName) :
	connection_(proxyDatabaseName)
{
}

CachedDataAccess::~CachedDataAccess()
{
}

void CachedDataAccess::getData(KvDataList &data, int station,
		const miutil::miTime &from, const miutil::miTime &to, int paramid,
		int type, int sensor, int lvl) const
{
	LogContext context("proxy_getData");
	//LOGDEBUG( "KvalobsProxy::proxy_getData" );

	// This should avoid problems caused by database entries '0' and 0:
	int alt_sensor;
	if (sensor >= '0')
		alt_sensor = sensor - '0';
	else
		alt_sensor = sensor + '0';

	ostringstream s;
	s << "select * from data where stationid=" << station << " and paramid="
			<< paramid << " and typeid=" << type
	//<< " and sensor=" << ((sensor < 10) ? (sensor + '0') : sensor)
			<< " and (sensor=" << sensor << " or sensor=" << alt_sensor << ")"
			<< " and level=" << lvl;
	if (from < to)
		s << " and (obstime>\'" << from << "\' and obstime<=\'" << to << "\')";
	else if (from == to)
		s << " and obstime=\'" << from << "\'";
	else
		// This is really an error, but...
		s << " and (obstime>\'" << to << "\' and obstime<=\'" << from << "\')";

	//LOGDEBUG( s.str() );

	try
	{
		auto_ptr<Result> res;
		Lock lock(proxy_mutex);
		res.reset(connection_.get().execQuery(s.str()));
		while (res->hasNext())
			data.push_back(kvalobs::kvData(res->next()));
	} catch (exception & e)
	{
		LOGERROR(e.what());
	} catch (...)
	{
		LOGERROR("Unknown error during database lookup");
	}
}

CKvalObs::CDataSource::Result_var CachedDataAccess::sendData(
		const KvDataList & data)
{
	for (CIKvDataList d = data.begin(); d != data.end(); d++)
	{
		string insertQuery = "insert into data values " + d->toSend();

		Lock lock(proxy_mutex);
		try
		{
			connection_.get().exec(insertQuery);
		} catch (exception &ex)
		{ // Should have been: dnmi::db::SQLDuplicate &ex ) {
			try
			{
				connection_.get().exec("delete from data " + d->uniqueKey());
				connection_.get().exec(insertQuery);
			} catch (exception &ex)
			{
				// proxy.connection_.get().rollBack();
				LOGERROR("Could not insert data! Error: " << ex.what());
			} catch (...)
			{
				// proxy.connection_.get().rollBack();
				LOGERROR("Could not insert data!");
			}
		} catch (...)
		{
			LOGERROR("Error: Unknown reason. Could (probably) not insert data.");
		}
	}
	return new CKvalObs::CDataSource::Result;
}

void CachedDataAccess::clear()
{
	LOGDEBUG("Deleting all entries in cache database");
	Lock lock(proxy_mutex);
	connection_.get().exec("delete from data");
	LOGINFO("Cache cleared");
}

void CachedDataAccess::deleteOldData(const miutil::miTime & olderThanThis)
{
	ostringstream query;
	query << "delete from data where obstime < \'" << olderThanThis << "\';";

	Lock lock(proxy_mutex);
	connection_.get().exec(query.str());
}

}
