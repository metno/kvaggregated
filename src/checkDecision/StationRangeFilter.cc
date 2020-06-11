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

#include "StationRangeFilter.h"
#include <kvalobs/kvData.h>
#include <boost/assign/list_of.hpp>
#include <set>
#include <sstream>

namespace aggregator
{

namespace
{
const std::set<int> precipitationParameters = boost::assign::list_of(104)(105)(106)(109)(110);
}


bool StationRangeFilter::shouldRunChecksOn(Metrics &m, const kvalobs::kvData & sourceData,
		const DataList &, std::string & msgOut )
{
	if ( sourceData.stationID() >= minStationId_ and
			sourceData.stationID() <= maxStationId_ and
			whiteList_.find(sourceData.paramID()) == whiteList_.end() )
	{
		std::ostringstream msg;
		msg << "Will not process station with id >= " << minStationId_ << " and not whitelisted parameters";
		msgOut = msg.str();
		return false;
	}
	return true;
}

}
