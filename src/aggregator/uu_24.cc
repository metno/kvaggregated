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

#include "uu_24.h"
#include "paramID.h"
#include <milog/milog.h>
#include <algorithm>


namespace aggregator
{

uu_24::uu_24() :
		MeanValueAggregator(UU, UUM_24)
{
}

uu_24::~uu_24()
{
}

namespace
{
struct have_obstime
{
	const miutil::miTime t_;
	have_obstime(const miutil::miTime & t) : t_(t) {}

	bool operator () (const kvalobs::kvData & d) const
	{
		return d.obstime() == t_;
	}
};

bool obstimeInList(const miutil::miTime & t, const AbstractAggregator::kvDataList & observations )
{
	return std::find_if(observations.begin(), observations.end(), have_obstime(t)) != observations.end();
}

bool matchingObsTimes(const uu_24::kvDataList & observations, const miutil::miClock & firstObs = "06:00:00")
{
	const miutil::miDate d = observations.front().obstime().date();

	miutil::miClock c = firstObs;
	for ( int i = 0; i < 3; ++ i )
	{
		miutil::miTime t(d, c);
		if ( not obstimeInList(t, observations) )
			return false;
		c.addHour(6);
	}
	return true;
}
}

bool uu_24::shouldProcess( const kvalobs::kvData &trigger, const kvDataList & observations ) const
{
	if ( MeanValueAggregator::shouldProcess(trigger, observations) )
		return true;

	int offsetFrom6 = trigger.obstime().hour() % 6;
	if ( offsetFrom6 == 0 )
		return matchingObsTimes(observations);
	else if ( offsetFrom6 == 1 )
		return observations.size() == 3 and matchingObsTimes(observations, "07:00:00");
	return false;
}

float uu_24::calculate(const std::vector<float> & source, const kvalobs::kvData & trigger) const
{
	if ( source.size() == 3 )
	{
		float factor = getStationMetadata("koppen", trigger);

		float c = factor;
		float q = (source[0] + source[2]) / 2.0;
		return q + (c * (source[1] - q));
	}
	return MeanValueAggregator::calculate(source, trigger);
}

namespace
{
struct have_obshour
{
	int hour_;
	have_obshour(int hour) : hour_(hour) {}
	bool operator () (const kvalobs::kvData & d)
	{
		return d.obstime().hour() == hour_;
	}
};

bool lt_obstime(const kvalobs::kvData & a, const kvalobs::kvData & b)
{
	return a.obstime() < b.obstime();
}
}

void uu_24::extractUsefulData(kvDataList & out, const kvDataList & dataIn, const kvalobs::kvData & trigger) const
{
	if ( MeanValueAggregator::shouldProcess(trigger, dataIn) )
		out = dataIn;
	else
	{
		const int offsetFrom6 = trigger.obstime().hour() % 6;
		for ( int i = 6 + offsetFrom6; i < 24; i += 6 )
		{
			kvDataList::const_iterator find = std::find_if(dataIn.begin(), dataIn.end(), have_obshour(i));
			if ( find == dataIn.end())
				throw std::runtime_error("Unable to find required data"); // should never happen
			out.push_back(* find);
		}
	}
}


}
