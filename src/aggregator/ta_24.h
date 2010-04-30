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

	virtual float calculateWithKoppensFormula(const std::vector<float> & source, float koppenFactor, ExtraData extraData) const;

	virtual ExtraData getExtraData(const kvalobs::kvData & data);

	class ExtraCalculationData;

private:
	const kvservice::DataAccess * dataAccess_;
};

class ta_24::ExtraCalculationData : public KoppenExtraData
{
public:
	explicit ExtraCalculationData(const kvalobs::kvData & trigger);

	virtual float minimumTemperature(const kvservice::DataAccess * dataAccess);

private:
	void populate(const kvservice::DataAccess * dataAccess);

	float tan06[2];
	float tan18[2];
	int useCount;
};


}

#endif /* TA_24_H_ */
