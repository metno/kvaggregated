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

#include "ot_24.h"
#include <paramID.h>
#include <times.h>
#include <numeric>

namespace aggregator
{

ot_24::ot_24() :
		StandardAggregator(OT_1, OT_24, 24, midnight)
{
}

bool ot_24::shouldProcess(const kvalobs::kvData &trigger, const kvDataList &observations) const
{
	kvDataList relevant;
	extractUsefulData(relevant, observations, trigger);
	return StandardAggregator::shouldProcess(trigger, relevant);
}

void ot_24::extractUsefulData(kvDataList & out, const kvDataList & dataIn, const kvalobs::kvData &) const
{
	for ( kvDataList::const_iterator it = dataIn.begin(); it != dataIn.end(); ++ it )
		if ( it->obstime().time_of_day().minutes() == 0 and it->obstime().time_of_day().seconds() == 0 )
			out.push_back(* it);
}

float ot_24::calculate(const ValueList & source, CalculationDataType, ExtraData) const
{
	return std::accumulate(source.begin(), source.end(), 0.0);
}

}
