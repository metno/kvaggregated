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

#include "MeanValueAggregator.h"
#include "times.h"
#include <numeric>

namespace aggregator
{

MeanValueAggregator::MeanValueAggregator(int readParam, int writeParam) :
		StandardAggregator(readParam, writeParam, 24, elevenPm)
{
}

MeanValueAggregator::~MeanValueAggregator()
{
}

bool MeanValueAggregator::shouldProcess( const kvalobs::kvData &trigger, const ParameterSortedDataList & observations ) const
{
	const AbstractAggregator::kvDataList & primaryObs = observations.find(primaryReadParam())->second;
	switch ( primaryObs.size() )
	{
	case 24:
		return true;
	case 8:
		for ( kvDataList::const_iterator it = primaryObs.begin(); it != primaryObs.end(); ++ it )
			if ( it->obstime().hour() % 3 != 0 )
				return false;
		return true;
	}
	return false;
}


void MeanValueAggregator::extractUsefulData(kvDataList & out, const kvDataList & dataIn, const kvalobs::kvData & trigger) const
{
	out = dataIn;
}

float MeanValueAggregator::calculate(const ValueList & source, ExtraData extraData ) const
{
	return std::accumulate(source.begin(), source.end(), 0.0) / source.size();
}

}
