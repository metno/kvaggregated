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

#include "KvalobsDataAccess.h"
#include "KvDataReceiver.h"
#include "metrics.h"
#include <decodeutility/kvalobsdataserializer.h>
#include <kvcpp/KvApp.h>
#include <milog/milog.h>

namespace kvservice
{

KvalobsDataAccess::KvalobsDataAccess()
{
}

KvalobsDataAccess::~KvalobsDataAccess()
{
}

namespace
{
  struct invalid
  {
    const int paramid;
    const int type;
    const int sensor;
    const int lvl;
    invalid( int paramid, int type, int sensor, int lvl )
        : paramid( paramid ), type( type ), sensor( sensor ), lvl( lvl )
    {}
    bool operator() ( const kvalobs::kvData & data )
    {
      return not ( paramid == data.paramID() and
                   type == data.typeID() and
                   //sensor  == data.sensor() and
                   ( sensor == data.sensor() or abs( sensor - data.sensor() ) == '0' ) and
                   lvl == data.level() );
    }
  };
}


void KvalobsDataAccess::getData(KvDataList &data, int station, const boost::posix_time::ptime &from,
		const boost::posix_time::ptime &to, int paramid, int type, int sensor, int lvl) const
{
	KvDataList tmpData;

	getAllData(tmpData, from, to, station);

    tmpData.remove_if( invalid( paramid, type, sensor, lvl ) );
    data.insert(data.end(), tmpData.begin(), tmpData.end());
}

void KvalobsDataAccess::getAllData(KvDataList & data, const boost::posix_time::ptime &from, const boost::posix_time::ptime &to, int station) const
{
		auto metrics = getMetrics();
		metrics->kvDb.start();

    WhichDataHelper wdh( CKvalObs::CService::All );
    boost::posix_time::ptime newFrom = from;
    if ( from != to )
      newFrom += boost::posix_time::seconds(1); // We don't want inclusive from

    wdh.addStation( station, newFrom, to );

    proxy::internal::KvDataReceiver dr( data );
    if ( ! KvApp::kvApp ) {
			metrics->kvDb.stop(true);
    	throw std::runtime_error("kvApp not initialized");
}

    bool result = KvApp::kvApp->getKvData( dr, wdh );

    if ( ! result ) {
      LOGERROR( "Unable to retrieve data from kvalobs." );
		}

		metrics->kvDb.stop(true);
}


CKvalObs::CDataSource::Result_var KvalobsDataAccess::sendData(const KvDataList & data)
{
	if ( ! KvApp::kvApp )
		throw std::runtime_error("No kvalobs connection");

	auto metrics=getMetrics();
	
	if( data.begin() != data.end() ) {
		metrics->sendtToKvalobs(1);
	} else {
		LOGWARN("Sending data to kvalobs: (no data to send)");
		metrics->sendtToKvalobs(-1);
	}
	
	metrics->kvDb.start();
	kvalobs::serialize::KvalobsData toSend;
	toSend.insert(data.begin(), data.end());
	toSend.overwrite(true);

	std::string msg = kvalobs::serialize::KvalobsDataSerializer::serialize(toSend);

//	miutil::miString msg = decodeutility::kvdataformatter::createString(data);
	LOGINFO( "Sending data to kvalobs:\n" << msg );
	auto res = KvApp::kvApp->sendDataToKv(msg.c_str(), "kv2kvDecoder");
	metrics->kvDb.stop(true);
	return res;
}

namespace // helper functions for getStationMetadata
{
bool compatible(const kvalobs::kvStationMetadata & metadata, const kvalobs::kvData & d)
{
	if ( metadata.stationID() != d.stationID() )
		return false;
	if ( not metadata.fromtime().is_not_a_date_time() and metadata.fromtime() > d.obstime() )
		return false;
	if ( not metadata.totime().is_not_a_date_time() and metadata.totime() < d.obstime() )
		return false;
	if ( metadata.haveSpecificParam() and metadata.paramID() != d.paramID() )
		return false;
	if ( metadata.haveSpecificType() and metadata.typeID() != d.typeID() )
		return false;
	if ( metadata.haveSpecificLevel() and metadata.level() != d.level() )
		return false;
	if ( metadata.haveSpecificSensor() and metadata.sensor() != d.sensor() )
		return false;
	return true;
}

int specificCount(const kvalobs::kvStationMetadata & metadata)
{
	int specificCount = 0;

	if (metadata.haveSpecificParam())
		++specificCount;
	if (metadata.haveSpecificType())
		++specificCount;
	if (metadata.haveSpecificLevel())
		++specificCount;
	if (metadata.haveSpecificSensor())
		++specificCount;
	return specificCount;
}

const kvalobs::kvStationMetadata * mostSpecific(const kvalobs::kvStationMetadata * a,
		const kvalobs::kvStationMetadata * b)
{
	if ( ! b )
		return a;
	if ( ! a )
		return b;
	if ( specificCount(* a) >= specificCount(* b) )
		return a;
	return b;
}
}

float KvalobsDataAccess::getStationMetadata(const std::string & metadataName, const kvalobs::kvData & validFor) const
{
	if ( ! KvApp::kvApp )
		throw std::runtime_error("No kvalobs connection have been established");

	auto metrics = getMetrics();
	metrics->kvDb.start();
	std::list<kvalobs::kvStationMetadata> metadata;

	if ( ! KvApp::kvApp->getKvStationMetaData(metadata, validFor.stationID(), validFor.obstime(), metadataName) ) {
		metrics->kvDb.stop(true);
		throw std::runtime_error("Unable to contact kvalobs");
	}

	const kvalobs::kvStationMetadata * ret = 0;
	for ( std::list<kvalobs::kvStationMetadata>::const_iterator it = metadata.begin(); it != metadata.end(); ++ it )
		if ( compatible(* it, validFor) )
			ret = mostSpecific(ret, &* it);

	if ( ! ret )
	{
		std::ostringstream msg;
		msg << "Unable to find metadata: " << metadataName << " for data " << validFor;
		metrics->kvDb.stop(true);
		throw std::runtime_error(msg.str());
	}

	metrics->kvDb.stop(true);
	return ret->metadata();
}

}
