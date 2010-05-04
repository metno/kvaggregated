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

#ifndef ABSTRACTAGGREGATOR_H_
#define ABSTRACTAGGREGATOR_H_

#include <kvalobs/kvData.h>
#include <list>
#include <memory>

namespace aggregator
{

class AbstractAggregator
{
public:
	AbstractAggregator(int readParam, int writeParam);
	virtual ~AbstractAggregator();

	/**
	 * \brief determine if we are interested in the given piece of data.
	 */
	virtual bool isInterestedIn(const kvalobs::kvData &data) const =0;

	typedef std::auto_ptr<kvalobs::kvData> kvDataPtr;
	typedef std::list<kvalobs::kvData> kvDataList;

	/**
	 * Perform an agregation, based on the incoming data, using the list
	 * observations as base data.
	 */
	virtual kvDataPtr process(const kvalobs::kvData & data, const kvDataList & observations) =0;


	/**
	 * \brief A time range.
	 */
	typedef std::pair<miutil::miTime, miutil::miTime> TimeSpan;

	/**
	 * \brief Find the earliest and latest interesting point in time
	 * which we are interested in.
	 *
	 * The return values are noninclusive for earliest time, and
	 * inclusive for latest time, so if this method returns the pair
	 * (18:00, 19:00), it indicates that we are interested in all data
	 * whith 18:00 \< validity \<= 19:00.
	 *
	 * \param data The data which triggered the call to this method.
	 *
	 * \return A pair of the times we are interested in.
	 */
	virtual const TimeSpan getTimeSpan(const kvalobs::kvData &data) const =0;


	/**
	 * \brief Get the value for paramID which we are interested in
	 * reading.
	 */
	int readParam() const
	{
		return read_param;
	}

	/**
	 * \brief Get the value for paramID which we are interested in
	 * writing.
	 */
	int writeParam() const
	{
		return write_param;
	}

private:
	const int read_param;
	const int write_param;
};

}

#endif /* ABSTRACTAGGREGATOR_H_ */
