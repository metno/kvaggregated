/*
 Kvalobs - Free Quality Control Software for Meteorological Observations

 $Id: ra2rr_12.cc,v 1.1.2.9 2007/09/27 09:02:16 paule Exp $

 Copyright (C) 2007 met.no

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
#include "ra2rr_12.h"
#include <kvalobs/kvDataOperations.h>
#include <milog/milog.h>
#include "times.h"

using namespace std;
using namespace kvalobs;

namespace aggregator
{
ra2rr_12::ra2rr_12() :
	StandardAggregator(RA, RR_12, 25, sixAmSixPm)
{
}

namespace
{
struct has_obstime
{
	const boost::posix_time::ptime obstime;
	has_obstime(const boost::posix_time::ptime & t) :
		obstime(t)
	{
	}
	bool operator ()(const kvData & d) const
	{
		return d.obstime() == obstime;
	}
};

template<int clock>
bool hasObsHour(const kvData & d)
{
	return boost::posix_time::hours(clock) == d.obstime().time_of_day();
}

bool hasObsHourSix(const kvData & d)
{
	return d.obstime().time_of_day() == boost::posix_time::hours(6) or
			d.obstime().time_of_day() == boost::posix_time::hours(18);
}

struct lt_obstime
{
	bool operator ()(const kvData & a, const kvData & b) const
	{
		return a.obstime() < b.obstime();
	}
};

}

bool ra2rr_12::shouldProcess(const kvData & trigger,
		const kvDataList & observations) const
{
	const set<boost::posix_time::time_duration> & gw = generateWhen();
	if (observations.size() > 1 and
			gw.find(trigger.obstime().time_of_day()) != gw.end() and
			find_if(observations.begin(), observations.end(),hasObsHour<6> ) != observations.end() and
			find_if(observations.begin(), observations.end(), hasObsHour<18> ) != observations.end())
		return true;
	return false;
}

float ra2rr_12::calculate(const ValueList & source, CalculationDataType, ExtraData ) const
{
	const float zero = 0.01;

	float result = 0;
	if (source.size() == 2) // don't have data for 24 hours ago
		result = source[1] - source[0];
	else if (source.size() == 3) // have data for 24 hours ago
	{
		const float diff12h = source[2] - source[1];
		if (diff12h < zero)
			result = 0;
		else
		{
			const float diff24h = source[2] - source[0];
			const float diff12hPrevious = source[1] - source[0];

			if (diff12hPrevious <= -100)
				result = diff12h;
			else if (diff24h <= zero)
				result = 0;
			else
			{
				if (diff12hPrevious >= zero)
					result = diff12h;
				else
					result = diff24h;
			}
		}
	}
	if (result < 0.0001)
		return 0;

	return result;
}

void ra2rr_12::extractUsefulData(kvDataList & out, const kvDataList & dataIn, const kvalobs::kvData & trigger) const
{
	kvDataList ret;
	for ( kvDataList::const_iterator it = dataIn.begin(); it != dataIn.end(); ++ it )
		if ( hasObsHour<6>(* it) or hasObsHour<18>(* it) )
			ret.push_back(* it);

	ret.sort(lt_obstime());

	if ( ret.size() == 2 )
	{
		const boost::posix_time::ptime & t1 = ret.front().obstime();
		const boost::posix_time::ptime & t2 = ret.back().obstime();
		//std::cout << boost::posix_time::ptime::hourDiff(t1, t2) << std::endl;
		if ( std::abs((t1 - t2).hours()) != std::abs(timeOffset()) )
			throw runtime_error("Missing middle period for ra generation");
	}
	else if ( ret.size() != 3 )
	{
		std::ostringstream msg;
		msg << "Unexpected number of observations: " << ret.size();
		throw runtime_error(msg.str());
	}

	out.swap(ret);
}

}
