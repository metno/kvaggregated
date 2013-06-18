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

#include "RR1OverridesRADecider.h"
#include <KvDataFunctors.h>
#include <paramID.h>
#include <proxy/DataAccess.h>
#include <kvalobs/kvData.h>
#include <kvalobs/kvDataOperations.h>

namespace aggregator
{

RR1OverridesRADecider::RR1OverridesRADecider(kvservice::DataAccess * dataAccess) :
		dataAccess_(dataAccess)
{
}

bool RR1OverridesRADecider::shouldRunChecksOn(const kvalobs::kvData & sourceData,
		const DataList & completeObservation, std::string & msgOut)
{
	if ( sourceData.paramID() == RA )
	{
		DataList::const_iterator find = std::find_if(
				completeObservation.begin(), completeObservation.end(),
				StationHasParamid(int(RR_1), & sourceData));

		if ( completeObservation.end() != find )
		{
			if ( rr1ShouldOverrideRa(sourceData, msgOut) )
			{
				msgOut = "Data contains both RA and RR_1, so we do not aggregate from RA";
				return false;
			}
			return true;
		}
	}
	else if ( sourceData.paramID() == RR_1 )
		return rr1ShouldOverrideRa(sourceData, msgOut);

	return true;
}

bool RR1OverridesRADecider::rr1ShouldOverrideRa(const kvalobs::kvData & sourceData, std::string & msgOut)
{
	if ( dataAccess_ )
	{
		kvservice::KvDataList data;

		boost::posix_time::ptime from = sourceData.obstime() - boost::posix_time::hours(11);
		boost::posix_time::ptime to = sourceData.obstime();
		dataAccess_->getData(data, sourceData.stationID(), from, to, RR_1, sourceData.typeID(), sourceData.sensor(), sourceData.level());

		if ( data.empty() )
			return false;

		kvservice::KvDataList raData;
		dataAccess_->getData(raData, sourceData.stationID(), from, to, RA, sourceData.typeID(), sourceData.sensor(), sourceData.level());
		if ( not raData.empty() )
		{
			for ( kvservice::KvDataList::const_iterator it = data.begin(); it != data.end(); ++ it )
				if ( kvalobs::original_missing(* it) or ! kvalobs::valid(* it) )
				{
					msgOut = "Data contains both RA and RR_1, but some RR_1 values are invalid, so instead we aggregate from RA";
					return false;
				}
		}
		return true;
	}
	return true;
}

}
