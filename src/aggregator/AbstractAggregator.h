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
#include <boost/shared_ptr.hpp>
#include <list>
#include <vector>

namespace aggregator
{

class AbstractAggregator
{
public:
	AbstractAggregator(int readParam, int writeParam);
	virtual ~AbstractAggregator();

	/**
	 * \brief determine if we are interested in the given piece of data, based on its obstime
	 */
	virtual bool isInterestedIn(const kvalobs::kvData &data) const { return true; }

	typedef boost::shared_ptr<kvalobs::kvData> kvDataPtr;
	typedef std::list<kvalobs::kvData> kvDataList;

	typedef std::map<int, AbstractAggregator::kvDataList> ParameterSortedDataList;

	/**
	 * Perform an agregation, based on the incoming data, using the list
	 * observations as base data.
	 */
	virtual kvDataPtr process(const kvalobs::kvData & data,
			const ParameterSortedDataList & observations) =0;


	/**
	 * \brief A time range.
	 */
	typedef std::pair<boost::posix_time::ptime, boost::posix_time::ptime> TimeSpan;

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


	typedef std::vector<int> ParameterList;

	/**
	 * Get the paramIDs which we are interested in reading.
	 */
	const ParameterList & readParam() const
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

protected:
	/**
	 * Get the paramIDs which we are interested in reading - for editing
	 */
	void addAdditionalReadParam(int parameter)
	{
		read_param.push_back(parameter);
	}

	/**
	 * \brief Get a data object for the created agregate.
	 *
	 * Return an agregate object for sending to kvalobs.
	 *
	 * \param trigger  The piece of data which triggered the call to
	 * this object.
	 *
	 * \param obsTime The time for the generated object.
	 *
	 * \param original The original agregated value for the observation. This may
	 * have the value \c invalidParam.
	 *
	 * \param corrected The corrected agregated value for the observation. This may
	 * have the value \c invalidParam.
	 *
	 * \param fagg Flag value to use in controlinfo(0)
	 *
	 * \return The agregate data object to be sent to kvalobs, or an
	 * empty object kvDataObj.clean() if the object is so similar to
	 * what already exists in kvalobs that it should not be sent.
	 */
	kvDataPtr
	getDataObject(const kvalobs::kvData &trigger,
			const boost::posix_time::ptime &obsTime, float original, float corrected, const std::list<kvalobs::kvData> & sourceData);

	/**
	 * \brief This is the kvalobs internal value for errors.
	 *
	 * If an error occurs which means that a correct value for an
	 * observation cannot be given, this should be returned instead.
	 *
	 * \note Incoming observations to kvalobs may also have this
	 * value. Each subclass must determine what to do if such a value
	 * is encountered.
	 */
	static const float invalidParam;

private:

	/**
	 * Calculate value for new controlinfo(0) - fagg, based on the source data which
	 * aggregation was made on.
	 */
	int calculateAggregateFlag(const kvDataList & sourceData) const;


	ParameterList read_param;
	const int write_param;
	const std::set<boost::posix_time::time_duration> generate_when;
};

namespace internal
{
/**
 * Calculate value for fagg flag when data is aggregated.
 *
 * @param sourceData source data for aggregation
 * @return value from fagg
 */
int calculateAggregateFlag_(const AbstractAggregator::kvDataList & sourceData);
}

}

#endif /* ABSTRACTAGGREGATOR_H_ */
