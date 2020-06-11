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

namespace
{
inline std::string to_kvalobs_string(const boost::gregorian::date & d)
{
	std::ostringstream s;
	s << d.year() << '-'
			<< std::setfill('0') << std::setw(2) << std::right << d.month().as_number() << '-'
			<< std::setfill('0') << std::setw(2) << std::right << d.day();
	return s.str();
}
inline std::string to_kvalobs_string(const boost::posix_time::time_duration & t)
{
	return to_simple_string(t);
}
inline std::string to_kvalobs_string(const boost::posix_time::ptime & t, char separator = ' ')
{
	std::ostringstream s;
	s << to_kvalobs_string(t.date()) << separator << to_kvalobs_string(t.time_of_day());
	return s.str();
}
}


namespace kvservice
{

CachedDataAccess::CachedDataAccess(const std::string & proxyDatabaseName) :
	connection_(proxyDatabaseName)
{
}

CachedDataAccess::~CachedDataAccess()
{
}

void CachedDataAccess::getData(Metrics &m,KvDataList &data, int station,
		const boost::posix_time::ptime &from, const boost::posix_time::ptime &to, int paramid,
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

	ostringstream query;
	query << "select * from data where stationid=" << station << " and paramid="
			<< paramid << " and typeid=" << type
	//<< " and sensor=" << ((sensor < 10) ? (sensor + '0') : sensor)
			<< " and (sensor=" << sensor << " or sensor=" << alt_sensor << ")"
			<< " and level=" << lvl;
	if (from < to)
		query << " and (obstime>\'" << to_kvalobs_string(from) << "\' and obstime<=\'" << to_kvalobs_string(to) << "\')";
	else if (from == to)
		query << " and obstime=\'" << to_kvalobs_string(from) << "\'";
	else
		// This is really an error, but...
		query << " and (obstime>\'" << to_kvalobs_string(to) << "\' and obstime<=\'" << to_kvalobs_string(from) << "\')";

	LOGDEBUG( query.str() );

	try
	{
		m.db.start();
		auto_ptr<Result> res;
		{
			Mutex::scoped_lock lock(proxy_mutex);
			res.reset(connection_.get().execQuery(query.str()));
		}
		while (res->hasNext())
			data.push_back(kvalobs::kvData(res->next()));
	} catch (exception & e)
	{
		LOGERROR(e.what());
	} catch (...)
	{
		LOGERROR("Unknown error during database lookup");
	}
	m.db.stop(true);
}

CKvalObs::CDataSource::Result_var CachedDataAccess::sendData(Metrics &m,
		const KvDataList & data)
{
	for (CIKvDataList d = data.begin(); d != data.end(); d++)
	{
		string insertQuery = "insert into data values " + d->toSend();

		Mutex::scoped_lock lock(proxy_mutex);
		m.db.start();
		try
		{
			LOGDEBUG(insertQuery);
			
			connection_.get().exec(insertQuery);
			m.db.stop(true);
		} catch (exception &ex)
		{ // Should have been: dnmi::db::SQLDuplicate &ex ) {
			try
			{
				connection_.get().exec("delete from data " + d->uniqueKey());
				connection_.get().exec(insertQuery);
				m.db.stop();
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
	Mutex::scoped_lock lock(proxy_mutex);
	connection_.get().exec("delete from data");
}

void CachedDataAccess::deleteOldData(const boost::posix_time::ptime & olderThanThis)
{
	ostringstream query;
	query << "delete from data where obstime < \'" << to_kvalobs_string(olderThanThis) << "\';";

	Mutex::scoped_lock lock(proxy_mutex);
	connection_.get().exec(query.str());
}

}
