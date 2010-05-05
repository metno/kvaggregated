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

#ifndef KOPPENBASEDMEANVALUEAGGREGATOR_H_
#define KOPPENBASEDMEANVALUEAGGREGATOR_H_

#include "MeanValueAggregator.h"

namespace aggregator
{

/**
 * Will generate mean values, either by using average of hourly or
 * three-hourly observations, or by using koppen's formula.
 *
 * Subclasses will need to override calculateWithKoppensFormula, to do the
 * actual aggregation.
 */
class KoppenBasedMeanValueAggregator: public aggregator::MeanValueAggregator
{
public:
	KoppenBasedMeanValueAggregator(int readParam, int writeParam);
	virtual ~KoppenBasedMeanValueAggregator();

protected:
    virtual bool shouldProcess( const kvalobs::kvData &trigger, const ParameterSortedDataList &observations ) const;

    virtual void extractUsefulData(ParameterSortedDataList & out, const ParameterSortedDataList & dataIn, const kvalobs::kvData & trigger) const;

	virtual ExtraData getExtraData(const kvalobs::kvData & data);

	virtual float calculate(const ValueList & source, ExtraData extraData) const;

    virtual float calculateWithKoppensFormula(const ValueList & source, float koppenFactor, ExtraData extraData) const = 0;

    struct KoppenExtraData : public ExtraAggregationData
    {
    	kvalobs::kvData trigger;
    	explicit KoppenExtraData(const kvalobs::kvData & trigger) : trigger(trigger) {}
    };
};

}

#endif /* KOPPENBASEDMEANVALUEAGGREGATOR_H_ */
