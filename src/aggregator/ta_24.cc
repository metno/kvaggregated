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

#include "ta_24.h"
#include "paramID.h"
#include <proxy/DataAccess.h>
#include <milog/milog.h>
#include <numeric>

namespace aggregator
{

ta_24::ta_24(const kvservice::DataAccess * dataAccess) :
		KoppenBasedMeanValueAggregator(TA, TAM_24),
		dataAccess_(dataAccess)
{
}

ta_24::~ta_24()
{
}

float ta_24::calculateWithKoppensFormula(const ValueList & source, float koppenFactor, ExtraData extraData) const
{
	ExtraCalculationData * d = static_cast<ExtraCalculationData *>(extraData);

	float minTemperature = d->minimumTemperature(dataAccess_);

	float n = std::accumulate(source.begin(), source.end(), 0.0) / 3.0;
	return n - (koppenFactor * (n - minTemperature));
}

ta_24::ExtraData ta_24::getExtraData(const kvalobs::kvData & trigger)
{
	ExtraCalculationData * ret = new ExtraCalculationData(trigger);
	return ret;
}

ta_24::ExtraCalculationData::ExtraCalculationData(const kvalobs::kvData & trigger) :
		KoppenExtraData(trigger), useCount(-1)
{
}

float ta_24::ExtraCalculationData::minimumTemperature(const kvservice::DataAccess * dataAccess)
{
	if ( useCount ++ == -1 )
		populate(dataAccess);
	return std::min(tan06[useCount], tan18[useCount]);
}

void ta_24::ExtraCalculationData::populate(const kvservice::DataAccess * dataAccess)
{
	if ( ! dataAccess )
		throw std::runtime_error("TA_24 calculation missing access to data - unable to process");

	kvservice::KvDataList data;
	dataAccess->getData(data, trigger.stationID(),
			miutil::miTime(trigger.obstime().date(), "06:00.00"),
			miutil::miTime(trigger.obstime().date(), "18:00.00"),
			TAN_12, trigger.typeID(), trigger.sensor(), trigger.level());

	if ( data.size() < 2 )
		throw std::runtime_error("Unable to find both TAN_12 observations for period");

	tan06[0] = data.front().original();
	tan06[1] = data.front().corrected();
	tan18[0] = data.back().original();
	tan18[1] = data.back().corrected();
}


}
