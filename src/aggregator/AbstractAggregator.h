/*
 Kvalobs - Free Quality Control Software for Meteorological Observations

 $Id: AbstractAggregator.h,v 1.1.2.8 2007/09/27 09:02:15 paule Exp $

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
#ifndef __agregator__AbstractAgregator_h__
#define __agregator__AbstractAgregator_h__

#include <kvcpp/KvApp.h>
#include <kvalobs/kvData.h>
#include <kvalobs/kvStation.h>
#include <puTools/miTime.h>
#include <kvskel/datasource.hh>
#include <list>
#include <set>
#include <map>
#include <utility>
#include <memory>

namespace aggregator
{

/**
 * \brief An abstract base class for creating agregates of data.
 *
 * This class is invoked in order to create an agregate of a
 * specific type of incoming data, such as from rain per hour to
 * rain per 24 hours. In order to recognize data generated in this
 * way from "regular" data, generated data will get a typeid which
 * is the negative value of its source data. Thus, if an object
 * generated data based on a set of data with typeid 300, its
 * generated data will have typeid -300.
 *
 * In order to do this, a set of protected virtual methods have been
 * defined, which can be overridden by subclasses. All these methods
 * will be automatically invoked by the AbstractAggregator
 * object. Apart from the method \a extractUsefulData and \a calculate,
 * which are pure virtual methods and therefore must be overridden, the
 * methods have a default implementation. The protected methods are invoked
 * in the following order: \a getTimeSpan, \a shouldProcess,
 * \a extractUsefulData and \a process. See the documentation for these
 * methods for further details.
 *
 * \warning The contents of this class is not thread-safe. Care
 * should therefore be used if a subclass is to use multiple
 * threads.
 */
class AbstractAggregator
{
public:

	/**
	 * \brief A time range.
	 */
	typedef std::pair<miutil::miTime, miutil::miTime> TimeSpan;

	/**
	 * \brief Set up the basic parameters of the AbstractAggregator
	 * object.
	 *
	 * \param readParam The paramID that incoming data should
	 * have. All data which comes to this object has this paramID.
	 *
	 * \param writeParam The paramID of generated data.
	 *
	 * \param interestingHours How many hours of data back in time are
	 * we interested in?
	 *
	 * \param generateWhen The times of day when we will generate
	 * data. Agregates will only be generated for these times.
	 */
	AbstractAggregator(int readParam, int writeParam, int interestingHours,
			const std::set<miutil::miClock> & generateWhen);

	virtual ~AbstractAggregator();

	/**
	 * \brief determine if we are interested in the given piece of data. 
	 */
	bool isInterestedIn(const kvalobs::kvData &data) const;

	typedef std::list<kvalobs::kvData> kvDataList;

	typedef std::auto_ptr<kvalobs::kvData> kvDataPtr;

	/**
	 * Perform an agregation, based on the incoming data, using the list 
	 * observations as base data.
	 */
	kvDataPtr process(const kvalobs::kvData & data, const kvDataList & observations);

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

	/**
	 * \brief Get the number of hours back in time we are interested
	 * in for generating an agregate observation.
	 */
	int interestingHours() const
	{
		return interesting_hours;
	}

	/**
	 * \brief Get the list of specifict times at which we want to
	 * generate agregate values.
	 */
	const std::set<miutil::miClock> & generateWhen() const
	{
		return generate_when;
	}

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
	virtual const TimeSpan getTimeSpan(const kvalobs::kvData &data) const;

protected:

	/**
	 * \brief Determine if enough data has been received in order to
	 * create an agregate.
	 *
	 * If this method return false, control will be returned to the
	 * caller, without any attempt having been made to generate an
	 * agregate value.
	 *
	 * The default implementation returns true if \a
	 * observations.size() >= \a interestingHours.
	 *
	 * \param trigger The piece of data which triggered the call to
	 * this object.
	 *
	 * \param observations The list returned by \a getRelevantObsList.
	 *
	 * \return True if we should proceed with calculating an agregate,
	 * False otherwise.
	 */
	virtual bool
	shouldProcess(const kvalobs::kvData &trigger,
			const kvDataList &observations) const;

	/**
	 * Extract exactly all data which is needed for aggregating.
	 *
	 * @throws exception if unable to find all needed data
	 *
	 * @param out The needed data goes here
	 * @param dataIn source data to select from
	 * @param trigger the piece of data which caused this aggregation to start.
	 */
	virtual void extractUsefulData(kvDataList & out, const kvDataList & dataIn,
			const kvalobs::kvData & trigger) const =0;

	struct ExtraAggregationData {
		virtual ~ExtraAggregationData() {}
	};
	typedef ExtraAggregationData * ExtraData;

	virtual ExtraData getExtraData(const kvalobs::kvData & data) { return 0; }

	typedef std::vector<float> ValueList;
	/**
	 * Do the actual aggregation.
	 *
	 * @param source base data for aggregating
	 * @param trigger The observation which caused this aggregation to run.
	 * @return the aggregated value
	 */
	virtual float calculate(const ValueList & source, ExtraData extraData) const = 0;

	/**
	 * Calculate value for new useinfo flag, based on the source data, which
	 * aggregation was made on.
	 *
	 * \warning Unless AGGREGATE_USEINFO is defined, this method will never be
	 *          called explicitly.
	 */
	kvalobs::kvUseInfo calculateUseInfo(const kvDataList & sourceData) const;

	/**
	 * Get station metadata from kvalobs. This i a service function to
	 * subclasses. Will search the kvalobs database for metadata with the
	 * given name, which it applicable to the given kvData object. Data is
	 * fetched from the station_metadata table.
	 *
	 * This method is virtual in order to make it overrideable by tests.
	 *
	 * \throws std::runtime_error if unable to find metadata, or if there is
	 * an error when contacting kvalobs.
	 *
	 * \param metadataname Name of the metadata to fetch
	 * \param validFor The object this metadata will be applied to.
	 *
	 * \return The value of the given metadata
	 */
	virtual float getStationMetadata(const std::string & metadataName, const kvalobs::kvData & validFor) const;

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
	static const float invalidParam = -32768;

	/**
	 * \brief A generated name for this object. This will be printed
	 * in front of all logging information given by this class.
	 *
	 * Cannot be const, because subclasses may change this name.
	 */
	std::string name;

private:
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
	 * \param agregateValue The agregated value for the observation. This may
	 * have the value \c invalidParam.
	 *
	 * \return The agregate data object to be sent to kvalobs, or an
	 * empty object kvDataObj.clean() if the object is so similar to
	 * what already exists in kvalobs that it should not be sent.
	 */
	kvalobs::kvData
	getDataObject_(const kvalobs::kvData &trigger,
			const miutil::miTime &obsTime, float original, float corrected, const kvalobs::kvUseInfo & ui);

	float generateOriginal_(const kvDataList & data, ExtraData extraData) const;
	float generateCorrected_(const kvDataList & data, ExtraData extraData) const;

	const int read_param;
	const int write_param;
	const int interesting_hours;
	const std::set<miutil::miClock> generate_when;
};
}

#endif // __agregator__AbstractAgregator_h__
