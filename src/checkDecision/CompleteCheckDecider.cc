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
#include "ForeignStationPrecipitationFilter.h"
#include "RaOverrideDecider.h"
#include <boost/assign/list_of.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

namespace aggregator
{
namespace
{
typedef boost::shared_ptr<RunCheckDecider> RunCheckDeciderPtr;
typedef std::vector<RunCheckDeciderPtr> DeciderList;

DeciderList deciders = boost::assign::list_of
		(RunCheckDeciderPtr(new ForeignStationPrecipitationFilter))
		(RunCheckDeciderPtr(new RaOverrideDecider))
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
