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

#include "CompleteCheckDecider.h"
#include "StationRangeFilter.h"
#include "RR1OverridesRADecider.h"
#include <vector>
#include <limits>

namespace aggregator
{
namespace
{
const int foreignStationWhiteListCount = 5;
const int foreignStationWhiteList[foreignStationWhiteListCount] = {104, 105, 106, 109, 110};

}

CompleteCheckDecider::CompleteCheckDecider(kvservice::DataAccess * dataAccess) :
		dataAccess_(dataAccess)
{
	// Todo: make this depend on a configuration file
	deciders_ = {
			RunCheckDeciderPtr(new StationRangeFilter(100000, std::numeric_limits<int>::max(), foreignStationWhiteList, foreignStationWhiteList + foreignStationWhiteListCount)),
			RunCheckDeciderPtr(new RR1OverridesRADecider(dataAccess_))
	};
}

bool CompleteCheckDecider::shouldRunChecksOn(Metrics &m, const kvalobs::kvData & sourceData,
		const DataList & completeObservation, std::string & msgOut)
{
	for ( DeciderList::iterator it = deciders_.begin(); it != deciders_.end(); ++ it )
		if ( not (*it)->shouldRunChecksOn(m, sourceData, completeObservation, msgOut) )
			return false;
	return true;
}

}
