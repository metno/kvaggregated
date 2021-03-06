/*
 Kvalobs - Free Quality Control Software for Meteorological Observations

 $Id: minmax.h,v 1.1.2.4 2007/09/27 09:02:16 paule Exp $

 Copyright (C) 2007 met.no

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
#ifndef __agregator_minmax_h__
#define __agregator_minmax_h__

#include "StandardAggregator.h"
#include "paramID.h"
#include <kvalobs/kvData.h>
#include <vector>

namespace aggregator
{
/**
 * \brief A min or max function.
 *
 * Usually std::min or std::max from the header \<algorthm\>
 */
typedef const float& (* Func)(const float&, const float&);

/**
 * \brief Calculates minimum and maximum values for observations.
 */
class MinMax: public StandardAggregator
{
	/**
	 * \brief The function to be called for doing agregate calculation.
	 */
	Func function;
public:

	/**
	 * \brief Constructor.
	 *
	 * \param readParam The paramID that incoming data should
	 * have.
	 *
	 * \param writeParam The paramID of generated data.
	 *
	 * \param interestingHours How many hours of data back in time are
	 * we interested in?
	 *
	 * \param generateWhen The times of day when we will generate
	 * data. Agregates will only be generated for these times.
	 *
	 * \param minmax A function pointer specifying what to do with the
	 * observations. The function is used as follows: for each
	 * observation: val = minmax(val, nexObservation)
	 */
	MinMax(int readParam, int writeParam, int interestingHours, const std::set<
			boost::posix_time::time_duration> &generateWhen, Func minmax);

    virtual bool shouldProcess( const kvalobs::kvData &trigger, const kvDataList &observations ) const;

	virtual void extractUsefulData(kvDataList & out, const kvDataList & dataIn, const kvalobs::kvData & trigger) const;

	virtual float calculate(const ValueList & source, CalculationDataType calcDataType, ExtraData extraData) const;
};

/**
 * \brief Get a MinMax object for calculating minimum value of all
 * observation.
 *
 * \param readParam The paramID that incoming data should
 * have.
 *
 * \param writeParam The paramID of generated data.
 *
 * \param hours How many hours of data back in time are we
 * interested in?
 *
 * \param when The times of day when we will generate
 * data. Agregates will only be generated for these times.
 *
 * \return A MinMax object for calculating minimum value of all
 * observation.
 */
inline MinMax min(int readParam, int writeParam, int hours, const std::set<boost::posix_time::time_duration> &when)
{
	return MinMax(readParam, writeParam, hours, when, std::min<float>);
}

/**
 * \brief Get a MinMax object for calculating maximum value of all
 * observation.
 *
 * \param readParam The paramID that incoming data should
 * have.
 *
 * \param writeParam The paramID of generated data.
 *
 * \param hours How many hours of data back in time are we
 * interested in?
 *
 * \param when The times of day when we will generate
 * data. Agregates will only be generated for these times.
 *
 * \return A MinMax object for calculating maximum value of all
 * observation.
 */
inline MinMax max(int readParam, int writeParam, int hours, const std::set<
		boost::posix_time::time_duration> &when)
{
	return MinMax(readParam, writeParam, hours, when, std::max<float>);
}
}

#endif // __agregator_minmax_h__
