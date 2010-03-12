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

#include "rr_24.h"
#include "times.h"
#include <kvalobs/kvDataOperations.h>

using namespace miutil;
using namespace kvalobs;


namespace agregator
{

rr_24::rr_24() :
	rr(RR_12, RR_24, 24, sixAm)
{
}

bool rr_24::shouldProcess(const kvData &trigger, const kvDataList &observations)
{
	// These are the times from which we will generate data:
	const std::set<miClock> &when = sixAmSixPm;

	const miTime &time = trigger.obstime();
	if (when.find(time.clock()) == when.end())
		return false;

	for (std::set<miClock>::const_iterator genTime = when.begin(); genTime
			!= when.end(); genTime++)
	{
		kvDataList::const_iterator search = observations.begin();
		while (search != observations.end())
		{
			const miTime &t = search->obstime();
			if (t.clock() == *genTime)
				break;
			search++;
		}
		if (search == observations.end())
			return false;
	}
	return true;
}

float rr_24::generateKvData(const kvDataList &data, const kvData &trigger)
{
	if (not valid(trigger))
		return invalidParam;

	const std::set<miClock> &when = sixAmSixPm;
	kvDataList relevant;

	for (std::set<miClock>::const_iterator it = when.begin(); it != when.end(); it++)
	{
		for (kvDataList::const_iterator dataIt = data.begin(); dataIt
				!= data.end(); dataIt++)
		{
			miTime t = dataIt->obstime();
			if (t.clock() == *it)
				relevant.push_back(*dataIt);
		}
	}
	return rr::generateKvData(relevant, trigger);
}

}
