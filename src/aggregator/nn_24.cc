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

#include "nn_24.h"
#include "paramID.h"

namespace aggregator
{

nn_24::nn_24() :
	MeanValueAggregator(NN, NNM_24)
{
}

nn_24::~nn_24()
{
}

bool nn_24::shouldProcess( const kvalobs::kvData &trigger, const ParameterSortedDataList &observations ) const
{
	const AbstractAggregator::kvDataList & primaryObs = observations.find(primaryReadParam())->second;
	return primaryObs.size() >= 3;
}

void nn_24::extractUsefulData(kvDataList & out, const kvDataList & dataIn, const kvalobs::kvData & trigger) const
{
	std::set<miutil::miTime> wantedTimes;
	wantedTimes.insert(miutil::miTime(trigger.obstime().date(), miutil::miClock(6,0,0)));
	wantedTimes.insert(miutil::miTime(trigger.obstime().date(), miutil::miClock(12,0,0)));
	wantedTimes.insert(miutil::miTime(trigger.obstime().date(), miutil::miClock(18,0,0)));

	for ( kvDataList::const_iterator it = dataIn.begin(); it != dataIn.end(); ++ it )
		if ( wantedTimes.find(it->obstime()) != wantedTimes.end() )
			out.push_back(* it);

	if ( out.size() != 3 )
		throw std::runtime_error("Unable to find correct periods for agregation");
}

}
