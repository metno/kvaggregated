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
using namespace miutil;
using namespace kvalobs;

namespace agregator
{
ra2rr_12::ra2rr_12() :
	AbstractAgregator(RA, RR_12, 25, sixAmSixPm)
{
}

namespace
{
struct has_obstime
{
	const miutil::miTime obstime;
	has_obstime(const miutil::miTime & t) :
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
	return clock == d.obstime().hour();
}

bool hasObsHourSix(const kvData & d)
{
	int hour = d.obstime().hour();
	return 6 == hour or 18 == hour;
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
		const kvDataList & observations)
{
	const set<miClock> & gw = generateWhen();
	if (observations.size() > 1 and
			gw.find(trigger.obstime().clock()) != gw.end() and
			find_if(observations.begin(), observations.end(),hasObsHour<6> ) != observations.end() and
			find_if(observations.begin(), observations.end(), hasObsHour<18> ) != observations.end())
		return true;
	return false;
}

float ra2rr_12::generateKvData(const kvDataList &data, const kvData & trigger)
{
	if (!valid(trigger))
		return invalidParam;

	const kvData * to = &trigger;

	miTime t = to->obstime();
	t.addHour(timeOffset());

	kvDataList::const_iterator find = find_if(data.begin(), data.end(),
			has_obstime(t));
	if (find == data.end())
		throw runtime_error("Could not find other relevant RA observation");
	const kvData * from = &*find;

	if (from->obstime() > to->obstime())
		swap(from, to);

	t = to->obstime();
	t.addDay(-1);
	kvDataList::const_iterator oneDayAgo = find_if(data.begin(), data.end(),
			has_obstime(t));

	return agregate(*from, *to, oneDayAgo == data.end() ? 0 : &*oneDayAgo);
}

float ra2rr_12::agregate(const kvData & from, const kvData & to,
		const kvData * oneDayAgo) const
{
	if (not (valid(from) and valid(to)))
		return invalidParam;

	std::vector<float> corrected;
	if ( oneDayAgo )
		corrected.push_back(oneDayAgo->corrected());
	corrected.push_back(from.corrected());
	corrected.push_back(to.corrected());

	return calculate(corrected);
}

float ra2rr_12::calculate(const std::vector<float> & source) const
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

}
