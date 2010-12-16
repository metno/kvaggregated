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

namespace aggregator
{

ot_24::ot_24() :
		StandardAggregator(OT_1, OT_24, 24, midnight)
{
}

void ot_24::extractUsefulData(kvDataList & out, const kvDataList & dataIn, const kvalobs::kvData &) const
{
	out = dataIn;
}

float ot_24::calculate(const ValueList & source, CalculationDataType, ExtraData) const
{
	float sum = 0;
	for ( ValueList::const_iterator it = source.begin(); it != source.end(); ++ it )
	{
		if ( * it < 0 )
			return invalidParam;
		if ( * it > 60 )
			return invalidParam;
		sum += * it;
	}
	return sum;
}

}
