/*
 Kvalobs - Free Quality Control Software for Meteorological Observations

 $Id: StandardAggregator.cc,v 1.1.2.9 2007/09/27 09:02:15 paule Exp $

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
#include "StandardAggregator.h"
//#include "AggregatorHandler.h"
#include "useinfoAggregate.h"
#include <proxy/KvalobsDataAccess.h>
#include <kvalobs/kvDataOperations.h>
#include <puTools/miTime.h>
#include <puTools/miString.h>
#include <milog/milog.h>
#include <decodeutility/kvDataFormatter.h>
#include <boost/functional.hpp>
#include <boost/scoped_ptr.hpp>
#include <sstream>
#include <vector>
#include <list>
#include <memory>
#include <stdexcept>
#include <cmath>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace kvservice;
using namespace kvalobs;
using namespace miutil;
using namespace dnmi::db;
using namespace milog;
using namespace boost;

namespace aggregator
{
StandardAggregator::StandardAggregator(int readParam, int writeParam,
		int interestingHours, const set<miClock> &generateWhen) :
		AbstractAggregator(readParam, writeParam),
	name("Agregator(" + lexical_cast<string> (readParam) + ", " + lexical_cast<
			string> (writeParam) + ")"), interesting_hours(interestingHours), generate_when(
			generateWhen)
{
}

StandardAggregator::~StandardAggregator()
{
}

const StandardAggregator::TimeSpan StandardAggregator::getTimeSpan(
		const kvData & data) const
{
	// Find out what times of day we are interested in:
	miTime time = data.obstime();
	miDate date = time.date();
	set<miClock>::const_iterator it = generate_when.lower_bound(time.clock());
	if (it == generate_when.end())
	{
		it = generate_when.begin();
		date.addDay();
	}
	miTime genTime(date, *it);
	miTime startTime = genTime;
	startTime.addHour(-interesting_hours);

	const TimeSpan ret(startTime, genTime);

	return ret;
}

bool StandardAggregator::shouldProcess(const kvalobs::kvData &trigger,
		const kvDataList &observations) const
{
	if ((int) observations.size() < interesting_hours)
		return false;
	return true;
}

kvalobs::kvData StandardAggregator::getDataObject_(const kvData &trigger,
		const miTime &obsTime, float original, float corrected, const kvalobs::kvUseInfo & ui)
{
	int typeID = trigger.typeID();
	if (typeID > 0)
		typeID *= -1;

	kvDataFactory f(trigger.stationID(), obsTime, typeID, trigger.sensor(),
			trigger.level());

	kvalobs::kvData ret = original == invalidParam ? f.getMissing(writeParam())
			: f.getData(original, writeParam());

	if (corrected == invalidParam)
		reject(ret);
	else if (original != corrected)
		correct(ret, corrected);

	ret.useinfo(ui);

	return ret;
}

bool StandardAggregator::isInterestedIn(const kvalobs::kvData &data) const
{
	LogContext context(name + " Station=" + lexical_cast<string> (
			data.stationID()));

	// What time range should we use as base data?
	TimeSpan times = getTimeSpan(data);

	// Immediatly return if we obviously are supposed to agregate
	if (data.obstime().hour() != 6 and data.obstime().hour() != 18)
	{
		miTime t = miTime::nowTime();
		t.addMin(30);
		if (data.obstime() < t)
		{
			t.addHour(2);
			if (times.second > t)
				return false;
		}
	}
	return true;
}

namespace
{
float round(float f)
{
	if ( f < 0 )
		f -= 0.05;
	else
		f += 0.05;
	f *= 10;
	f = int(f);
	return f / 10.0;
}
}

AbstractAggregator::kvDataPtr StandardAggregator::process(
		const kvalobs::kvData & data,
		const ParameterSortedDataList & p_observations)
{
	if ( p_observations.size() != 1 )
		throw std::logic_error("internal error - StandardAggregator may only have one parameter dependency.");

	ParameterSortedDataList::const_iterator find = p_observations.find(data.paramID());
	if ( find == p_observations.end())
		throw std::logic_error("Internal error");
	const kvDataList & observations = find->second;

	if (not shouldProcess(data, observations))
	{
		LOGDEBUG( "Will not process" );
		return kvDataPtr();
	}

	LOGINFO( "Agregating " << decodeutility::kvdataformatter::createString(data) );

	// Call abstract method to get agregate value:
	try
	{
		kvDataList relevantData;
		extractUsefulData(relevantData, observations, data);

		boost::scoped_ptr<ExtraAggregationData> extraData(getExtraData(data));

		float original = round(generateOriginal_(relevantData, extraData.get()));
		float corrected = round(generateCorrected_(relevantData, extraData.get()));

#ifdef AGGREGATE_USEINFO
		kvalobs::kvUseInfo ui = calculateUseInfo(relevantData);
#else
		kvalobs::kvUseInfo ui;
#endif

		//original = corrected; // revert to old behaviour

		TimeSpan times = getTimeSpan(data);

		// Create a data object for saving
		miTime t = miTime(times.second.date(), miClock(times.second.hour(), 0,
				0));

		kvDataPtr ret(
				new kvData(getDataObject_(data, t, original, corrected, ui)));

		return ret;
	} catch (exception & err)
	{
		if (err.what()[0] != '\0')
		{
			LOGERROR( err.what() );
		}
		return kvDataPtr();
	} catch (...)
	{
		LOGERROR( "Unrecognized error" );
		return kvDataPtr();
	}
}

kvalobs::kvUseInfo StandardAggregator::calculateUseInfo(
		const kvDataList & sourceData) const
{
	std::vector<kvalobs::kvUseInfo> ui;
	for (kvDataList::const_iterator it = sourceData.begin(); it
			!= sourceData.end(); ++it)
		ui.push_back(it->useinfo());

	return aggregateUseFlag(ui);
}

float StandardAggregator::getStationMetadata(const std::string & metadataName, const kvalobs::kvData & validFor) const
{
	kvservice::KvalobsDataAccess dataAccess;
	return dataAccess.getStationMetadata(metadataName, validFor);
}


float StandardAggregator::generateOriginal_(const kvDataList & data, ExtraData extraData) const
{
	kvDataList::const_iterator find = std::find_if(data.begin(), data.end(),
			original_missing);
	if (find != data.end())
		return invalidParam;

	ValueList values;
	for (kvDataList::const_iterator it = data.begin(); it != data.end(); ++it)
		values.push_back(it->original());

	return calculate(values, extraData);
}

float StandardAggregator::generateCorrected_(const kvDataList & data, ExtraData extraData) const
{
	kvDataList::const_iterator find = std::find_if(data.begin(), data.end(),
			boost::not1(valid));
	if (find != data.end())
		return invalidParam;

	ValueList values;
	for (kvDataList::const_iterator it = data.begin(); it != data.end(); ++it)
		values.push_back(it->corrected());

	return calculate(values, extraData);
}

}
