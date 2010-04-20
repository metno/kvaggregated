/*
 Kvalobs - Free Quality Control Software for Meteorological Observations

 $Id: minmax.cc,v 1.1.2.5 2007/09/27 09:02:16 paule Exp $

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
#include "minmax.h"
#include <kvalobs/kvDataOperations.h>
#include <limits>
#include <algorithm>
#include <stdexcept>

using namespace std;
using namespace miutil;
using namespace kvalobs;

namespace aggregator
{
MinMax::MinMax(int readParam, int writeParam, int interestingHours, const set<miClock> &generateWhen, Func minmax) :
	AbstractAggregator(readParam, writeParam, interestingHours, generateWhen),
			function(minmax)
{
}

bool MinMax::shouldProcess( const kvalobs::kvData &trigger, const kvDataList &observations )
{
	if ( ! AbstractAggregator::shouldProcess(trigger, observations) )
		return false;

	TimeSpan time = getTimeSpan(trigger);
	for ( kvDataList::const_iterator it = observations.begin(); it != observations.end(); ++ it )
		if ( it->obstime() <= time.first or time.second < it->obstime() )
			return false;
	return true;
}


void MinMax::extractUsefulData(kvDataList & out, const kvDataList & dataIn, const kvalobs::kvData & trigger) const
{
	out = dataIn;
}

float MinMax::calculate(const std::vector<float> & source, const kvalobs::kvData & ) const
{
	float ret = source.front();
	for ( std::vector<float>::const_iterator it = source.begin(); it != source.end(); ++ it )
		ret = function(ret, * it);
	return ret;
}

}
