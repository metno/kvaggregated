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
#include <kvalobs/kvDataOperations.h>
#include <milog/milog.h>
#include <numeric>

namespace aggregator
{

ta_24::ta_24(const kvservice::DataAccess * dataAccess) :
		KoppenBasedMeanValueAggregator(TA, TAM_24),
		dataAccess_(dataAccess)
{
//	addAdditionalReadParam(TAN_12);
}

ta_24::~ta_24()
{
}

float ta_24::calculateWithKoppensFormula(const ValueList & source, float koppenFactor, CalculationDataType calcDataType, ExtraData extraData) const
{
	ExtraCalculationData * d = static_cast<ExtraCalculationData *>(extraData);

	float minTemperature = d->minimumTemperature(dataAccess_, calcDataType);

	if ( minTemperature == d->missing_ )
		return invalidParam;

	float n = std::accumulate(source.begin(), source.end(), 0.0) / 3.0;

	float ret =  n - (koppenFactor * (n - minTemperature));

	return ret;
}

namespace
{
	bool hasFullHour(const kvalobs::kvData & d)
	{
		const auto& t = d.obstime().time_of_day();
		return t.minutes() == 0 && t.seconds() == 0;
	}

	/**
	 * Filter observations to only those with full hour timestamps
	 */
	ta_24::kvDataList filterByFullHour(const ta_24::kvDataList &observations)
	{
		ta_24::kvDataList ret;
		for (const auto& obs : observations) {
			if(hasFullHour(obs))
				ret.push_back(obs);
		}
		return ret;
	}
}

bool ta_24::shouldProcess( const kvalobs::kvData &trigger, const kvDataList &observations ) const 
{
	kvDataList filtered = filterByFullHour(observations);
	return KoppenBasedMeanValueAggregator::shouldProcess(trigger, filtered);
}

void ta_24::extractUsefulData(kvDataList & out, const kvDataList & dataIn,
			const kvalobs::kvData & trigger) const
{
	auto partiallyFiltered = filterByFullHour(dataIn);
	KoppenBasedMeanValueAggregator::extractUsefulData(out, partiallyFiltered, trigger);
}

ta_24::ExtraData ta_24::getExtraData(const kvalobs::kvData & trigger)
{
	ExtraCalculationData * ret = new ExtraCalculationData(trigger);
	return ret;
}

ta_24::ExtraCalculationData::ExtraCalculationData(const kvalobs::kvData & trigger) :
		KoppenExtraData(trigger), originalTan24(missing_), correctedTan24(missing_), gotData_(false)
{
}

float ta_24::ExtraCalculationData::minimumTemperature(const kvservice::DataAccess * dataAccess, CalculationDataType calcDataType)
{
	if ( not gotData_ )
	{
		populate(dataAccess);
		gotData_ = true;
	}

	if ( calcDataType == Original )
		return originalTan24;
	else
		return correctedTan24;
}

void ta_24::ExtraCalculationData::populate(const kvservice::DataAccess * dataAccess)
{
	if ( ! dataAccess )
		throw std::runtime_error("TA_24 calculation missing access to data - unable to process");

	kvservice::KvDataList data;
	dataAccess->getData(data, trigger.stationID(),
			boost::posix_time::ptime(trigger.obstime().date(), boost::posix_time::time_duration(5,59,59)),
			boost::posix_time::ptime(trigger.obstime().date(), boost::posix_time::hours(18)),
			TAN_12, trigger.typeID(), trigger.sensor(), trigger.level());

	if ( data.size() < 2 )
		throw std::runtime_error("Unable to find both TAN_12 observations for period");

	const kvalobs::kvData & a = data.front();
	const kvalobs::kvData & b = data.back();

	if ( kvalobs::original_missing(a) or kvalobs::original_missing(b) )
		originalTan24 = missing_;
	else
		originalTan24 = std::min(a.original(), b.original());

	if ( not kvalobs::valid(a) or not kvalobs::valid(b) )
		correctedTan24 = missing_;
	else
		correctedTan24 = std::min(a.corrected(), b.corrected() );
}

const float ta_24::ExtraCalculationData::missing_ = -32767;

}
