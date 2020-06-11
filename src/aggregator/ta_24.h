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

#ifndef TA_24_H_
#define TA_24_H_

#include "KoppenBasedMeanValueAggregator.h"

namespace kvservice
{
class DataAccess;
}

namespace aggregator
{

class ta_24: public KoppenBasedMeanValueAggregator
{
public:
	explicit ta_24(const kvservice::DataAccess * dataAccess);
	virtual ~ta_24();

	virtual float calculateWithKoppensFormula(Metrics &m, const ValueList & source, float koppenFactor, CalculationDataType calcDataType, ExtraData extraData) const;

	virtual ExtraData getExtraData(const kvalobs::kvData & data);

	class ExtraCalculationData;

private:
	const kvservice::DataAccess * dataAccess_;
};

class ta_24::ExtraCalculationData : public KoppenExtraData
{
public:
	explicit ExtraCalculationData(const kvalobs::kvData & trigger);

	float minimumTemperature(Metrics &m, const kvservice::DataAccess * dataAccess, CalculationDataType calcDataType);

	static const float missing_;
protected:
	virtual void populate(Metrics &m, const kvservice::DataAccess * dataAccess);

	bool gotData_;
	float originalTan24;
	float correctedTan24;
};


}

#endif /* TA_24_H_ */
