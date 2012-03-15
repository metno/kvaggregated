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
#include <boost/assign/list_of.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <limits>

namespace aggregator
{
namespace
{
typedef boost::shared_ptr<RunCheckDecider> RunCheckDeciderPtr;
typedef std::vector<RunCheckDeciderPtr> DeciderList;

const int foreignStationWhiteListCount = 5;
const int foreignStationWhiteList[foreignStationWhiteListCount] = {104, 105, 106, 109, 110};

// Todo: make this depend on a configuration file
DeciderList deciders = boost::assign::list_of
		(RunCheckDeciderPtr(new StationRangeFilter(100000, std::numeric_limits<int>::max(), foreignStationWhiteList, foreignStationWhiteList + foreignStationWhiteListCount)))
		(RunCheckDeciderPtr(new RR1OverridesRADecider))
	;
}


bool CompleteCheckDecider::shouldRunChecksOn(const kvalobs::kvData & sourceData,
		const DataList & completeObservation, std::string & msgOut)
{
	for ( DeciderList::iterator it = deciders.begin(); it != deciders.end(); ++ it )
		if ( not (*it)->shouldRunChecksOn(sourceData, completeObservation, msgOut) )
			return false;
	return true;
}

}
