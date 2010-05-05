/*
 Kvalobs - Free Quality Control Software for Meteorological Observations

 $Id: rr.cc,v 1.1.2.6 2007/09/27 09:02:16 paule Exp $

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
#include "rr.h"
#include <kvalobs/kvDataOperations.h>
#include <kvalobs/kvObsPgm.h>
#include <milog/milog.h>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <memory>
#include <vector>


using namespace std;
using namespace miutil;
using namespace kvalobs;
using namespace boost;

namespace aggregator
{

rr::rr(int readParam, int writeParam, int interestingHours,
		const set<miClock> &generateWhen) :
	StandardAggregator(readParam, writeParam, interestingHours, generateWhen)
{
}

void rr::extractUsefulData(ParameterSortedDataList & out, const ParameterSortedDataList & dataIn, const kvalobs::kvData & trigger) const
{
	out = dataIn;
}


float rr::calculate(const ValueList & source, ExtraData ) const
{
	bool nothing = true;
	float sum = 0;
	for ( ValueList::const_iterator it = source.begin(); it != source.end(); ++ it )
	{
		if (* it >= 0)
		{
			sum += * it;
			nothing = false;
		}
	}
	if ( nothing )
		return -1;
	return sum;
}


}
