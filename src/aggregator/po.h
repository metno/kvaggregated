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

#ifndef PO_H_
#define PO_H_

#include "metrics.h"
#include "AbstractAggregator.h"

namespace kvservice
{
class DataAccess;
}

namespace aggregator
{

class po: public aggregator::AbstractAggregator
{
public:
	explicit po(const kvservice::DataAccess & dataAccess);
	virtual ~po();

	virtual kvDataPtr process(Metrics &m, const kvalobs::kvData & data, const ParameterSortedDataList & observations);
	virtual kvDataPtr process(Metrics &m,const kvalobs::kvData & data, const ParameterSortedDataList & observations, const ParameterSortedDataList & alreadyAggregated);

	// Unused, awaiting a system for identifying inversion correcting stations:
	kvDataPtr processMethod2(Metrics &m,const kvalobs::kvData & data, const ParameterSortedDataList & observations);

	virtual const TimeSpan getTimeSpan(const kvalobs::kvData &data) const;

protected:
	// protected to be overrideable by tests
	virtual float getStationMetadata(Metrics &m, const std::string & metadataName, const kvalobs::kvData & validFor) const;

private:
	kvDataPtr process_(Metrics &m,const kvalobs::kvData & data, const ParameterSortedDataList & observations, int method);

	/**
	 * \param pr air pressure reduced to sea level
	 * \param ta air temperature
	 * \param um yearly mean air humidity
	 * \param tm yearly mean air temperature
	 * \param hp reference value for air pressure
	 */
	float computePo(float pr, float ta, float um, float tm, float hp) const;

	/**
	 * \param pr air pressure reduced to sea level
	 * \param ta air temperature
	 * \param um yearly mean air humidity
	 * \param tm yearly mean air temperature
	 * \param hp reference value for air pressure
	 */
	float computePoWithInversionCorrection(float pr, float ta, float um, float tm, float hp) const;

private:
	const kvservice::DataAccess & dataAccess_;
};

}

#endif /* PO_H_ */
