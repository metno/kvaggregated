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

#include "AbstractAggregatorTest.h"
#include <aggregator/ta_24.h>
#include <kvalobs/kvDataOperations.h>
#include <kvcpp/kvservicetypes.h>
#include <map>

using namespace aggregator;

namespace
{
boost::posix_time::ptime pt(const std::string & s)
{
	return boost::posix_time::time_from_string(s);
}
}


/**
 * Testing version of class ta_24. This class will merely pretend to contact
 * kvalobs on calls to getStationMetadata.
 */
class ta_24_test_version : public ta_24
{
	float stationMetadata_;
	float tan24_;
	bool throwOnCall_;
public:
	ta_24_test_version() :
		ta_24(0), stationMetadata_(0.75), tan24_(7.5), throwOnCall_(false)
	{}

	class TestExtraCalculationData : public ta_24::ExtraCalculationData
	{
	public:
		TestExtraCalculationData(const kvalobs::kvData & d, float tan24) : ExtraCalculationData(d)
		{
			originalTan24 = tan24;
			correctedTan24 = tan24;
		}

	protected:
		virtual void populate(const kvservice::DataAccess * dataAccess)	{}

	private:

	};

	virtual ExtraData getExtraData(const kvalobs::kvData & data)
	{
		return new TestExtraCalculationData(data, tan24_);
	}

	/**
	 * Any calls to getStationMetadata will after this call return the given
	 * value, unless setThrowOnCallToStationMetadata have been set to true
	 */
	void setNextReturnValueForStationMetadata(float valueToReturn)
	{
		stationMetadata_ = valueToReturn;
	}

	void setTan24(float val)
	{
		tan24_ = val;
	}

	/**
	 * Instead of returning a value when calling getStationMetadata, throw an
	 * exception. Exceptions from this method signals an error, either in
	 * contacting kvalobs, or because there was a request for a value which
	 * did not exist in the database.
	 */
	void setThrowOnCallToStationMetadata(bool doThrow = true)
	{
		throwOnCall_ = doThrow;
	}
protected:
	// Overrides getStationMetadata in StandardAggregator
	virtual float getStationMetadata(const std::string & metadataName, const kvalobs::kvData & validFor) const
	{
		if ( throwOnCall_ )
			throw std::runtime_error("Artificially generated error for testing");
		return stationMetadata_;
	}
};

class ta_24Test : public AbstractAggregatorTest
{
protected:
	ta_24Test() :
		factory(1, pt("2010-03-16 06:00:00"), 1)
	{}

	kvalobs::kvDataFactory factory;
	ta_24_test_version aggregator;
};

INSTANTIATE_TEST_CASE_P(ta_24Test, AbstractAggregatorTest, testing::Values(AggregatorPtr(new ta_24(0))));


TEST_F(ta_24Test, data24hours)
{
	AbstractAggregator::ParameterSortedDataList data;
	StandardAggregator::kvDataList & dl = data[aggregator.readParam().front()];

	for (boost::posix_time::ptime t = pt("2010-04-19 00:00:00"); t < pt("2010-04-20 00:00:00"); t += boost::posix_time::hours(1) )
		dl.push_back(factory.getData(t.time_of_day().hours(), aggregator.readParam().front(), t));
	kvalobs::correct(dl.front(), 3);

	ta_24::kvDataPtr result = aggregator.process(dl.front(), data);

	ASSERT_TRUE( result.get() );

	EXPECT_FLOAT_EQ(11.5, result->original());
	EXPECT_FLOAT_EQ(11.6, result->corrected());
}

TEST_F(ta_24Test, data8hours)
{
	AbstractAggregator::ParameterSortedDataList data;
	StandardAggregator::kvDataList & dl = data[aggregator.readParam().front()];

	for (boost::posix_time::ptime t = pt("2010-04-19 00:00:00"); t < pt("2010-04-20 00:00:00"); t += boost::posix_time::hours(3) )
		dl.push_back(factory.getData(t.time_of_day().hours(), aggregator.readParam().front(), t));
	kvalobs::correct(dl.front(), 3);

	ta_24::kvDataPtr result = aggregator.process(dl.front(), data);

	ASSERT_TRUE( result.get() );

	EXPECT_FLOAT_EQ(10.5, result->original());
	EXPECT_FLOAT_EQ(10.9, result->corrected());
}

TEST_F(ta_24Test, data3hoursWrongTimes)
{
	AbstractAggregator::ParameterSortedDataList data;
	StandardAggregator::kvDataList & dl = data[aggregator.readParam().front()];

	for (boost::posix_time::ptime t = pt("2010-04-19 00:00:00"); t < pt("2010-04-19 03:00:00"); t += boost::posix_time::hours(1) )
		dl.push_back(factory.getData(t.time_of_day().hours(), aggregator.readParam().front(), t));

	ASSERT_EQ(3u, dl.size()) << "test precondition error";

	ta_24::kvDataPtr result = aggregator.process(dl.front(), data);

	ASSERT_FALSE( result.get() );
}

TEST_F(ta_24Test, data3hoursStartAt7)
{
	AbstractAggregator::ParameterSortedDataList data;
	StandardAggregator::kvDataList & dl = data[aggregator.readParam().front()];

	dl.push_back(factory.getData(6, aggregator.readParam().front(), pt("2010-04-19 07:00:00")));
	dl.push_back(factory.getData(12, aggregator.readParam().front(), pt("2010-04-19 13:00:00")));
	dl.push_back(factory.getData(18, aggregator.readParam().front(), pt("2010-04-19 19:00:00")));
	kvalobs::correct(dl.front(), 3);

	ta_24::kvDataPtr result = aggregator.process(dl.front(), data);

	ASSERT_TRUE( result.get() );

	EXPECT_FLOAT_EQ(8.6, result->original());
	EXPECT_FLOAT_EQ(8.4, result->corrected());
}


TEST_F(ta_24Test, data3hoursMissingMetadata)
{
	// Simulate StandardAggregator method getStationMetadata not finding any data.
	aggregator.setThrowOnCallToStationMetadata();

	AbstractAggregator::ParameterSortedDataList data;
	StandardAggregator::kvDataList & dl = data[aggregator.readParam().front()];

	dl.push_back(factory.getData(6, aggregator.readParam().front(), pt("2010-04-19 06:00:00")));
	dl.push_back(factory.getData(12, aggregator.readParam().front(), pt("2010-04-19 12:00:00")));
	dl.push_back(factory.getData(18, aggregator.readParam().front(), pt("2010-04-19 18:00:00")));
	kvalobs::correct(dl.front(), 3);

	ta_24::kvDataPtr result = aggregator.process(dl.front(), data);

	ASSERT_FALSE( result.get() );
}

TEST_F(ta_24Test, data3hoursMissingTan12)
{
	aggregator.setTan24(ta_24::ExtraCalculationData::missing_);

	AbstractAggregator::ParameterSortedDataList data;
	StandardAggregator::kvDataList & dl = data[aggregator.readParam().front()];

	dl.push_back(factory.getData(6, aggregator.readParam().front(), pt("2010-04-19 06:00:00")));
	dl.push_back(factory.getData(12, aggregator.readParam().front(), pt("2010-04-19 12:00:00")));
	dl.push_back(factory.getData(18, aggregator.readParam().front(), pt("2010-04-19 18:00:00")));

	ta_24::kvDataPtr result = aggregator.process(dl.front(), data);

	ASSERT_TRUE( result.get() );

	EXPECT_TRUE(kvalobs::original_missing(* result));
	EXPECT_FALSE(kvalobs::valid(* result));
}


TEST_F(ta_24Test, data3hoursMissingOriginal)
{
	AbstractAggregator::ParameterSortedDataList data;
	StandardAggregator::kvDataList & dl = data[aggregator.readParam().front()];

	dl.push_back(factory.getData(6, aggregator.readParam().front(), pt("2010-04-19 06:00:00")));
	kvalobs::kvData d12 = factory.getMissing(aggregator.readParam().front(), pt("2010-04-19 12:00:00"));
	kvalobs::correct(d12, 12);
	dl.push_back(d12);
	dl.push_back(factory.getData(18, aggregator.readParam().front(), pt("2010-04-19 18:00:00")));

	ta_24::kvDataPtr result = aggregator.process(dl.front(), data);

	ASSERT_TRUE( result.get() );

	EXPECT_TRUE(kvalobs::original_missing(* result));
	EXPECT_FLOAT_EQ(8.6, result->corrected());
}


TEST_F(ta_24Test, data3hours)
{
	AbstractAggregator::ParameterSortedDataList data;
	StandardAggregator::kvDataList & dl = data[aggregator.readParam().front()];

	dl.push_back(factory.getData(6, aggregator.readParam().front(), pt("2010-04-19 06:00:00")));
	dl.push_back(factory.getData(12, aggregator.readParam().front(), pt("2010-04-19 12:00:00")));
	dl.push_back(factory.getData(18, aggregator.readParam().front(), pt("2010-04-19 18:00:00")));
	kvalobs::correct(dl.front(), 3);

	ta_24::kvDataPtr result = aggregator.process(dl.front(), data);

	ASSERT_TRUE( result.get() );

	EXPECT_FLOAT_EQ(8.6, result->original());
	EXPECT_FLOAT_EQ(8.4, result->corrected());
}


TEST_F(ta_24Test, data3hoursUnsorted)
{
	// What happens if the data we recieve is not sorted by time?

	aggregator.setNextReturnValueForStationMetadata(0.5);

	AbstractAggregator::ParameterSortedDataList data;
	StandardAggregator::kvDataList & dl = data[aggregator.readParam().front()];

	dl.push_back(factory.getData(12, aggregator.readParam().front(), pt("2010-04-19 12:00:00")));
	dl.push_back(factory.getData(8, aggregator.readParam().front(), pt("2010-04-19 06:00:00")));
	dl.push_back(factory.getData(16, aggregator.readParam().front(), pt("2010-04-19 18:00:00")));
	kvalobs::correct(dl.front(), 3);

	ta_24::kvDataPtr result = aggregator.process(dl.front(), data);

	ASSERT_TRUE( result.get() );

	EXPECT_FLOAT_EQ(9.8, result->original());
	EXPECT_FLOAT_EQ(8.3, result->corrected());
}

TEST_F(ta_24Test, inclomplete24HourObservationPossiblyInterpretedAs3hourObs)
{
	AbstractAggregator::ParameterSortedDataList data;
	StandardAggregator::kvDataList & dl = data[aggregator.readParam().front()];

	for (boost::posix_time::ptime t = pt("2010-04-19 00:00:00"); t < pt("2010-04-19 19:00:00"); t += boost::posix_time::hours(1) )
		dl.push_back(factory.getData(t.time_of_day().hours(), aggregator.readParam().front(), t));

	ta_24::kvDataPtr result = aggregator.process(dl.back(), data);

	ASSERT_TRUE( result.get() );

	EXPECT_FLOAT_EQ(8.6, result->original());
	EXPECT_FLOAT_EQ(8.6, result->corrected());
}

TEST_F(ta_24Test, inclomplete24HourObservationPossiblyInterpretedAs3hourObsTriggerAt7)
{
	// Generation at 7 should only happen when there are three observations

	AbstractAggregator::ParameterSortedDataList data;
	StandardAggregator::kvDataList & dl = data[aggregator.readParam().front()];

	for (boost::posix_time::ptime t = pt("2010-04-19 00:00:00"); t < pt("2010-04-19 20:00:00"); t += boost::posix_time::hours(1) )
		dl.push_back(factory.getData(t.time_of_day().hours(), aggregator.readParam().front(), t));

	ta_24::kvDataPtr result = aggregator.process(dl.back(), data);

	ASSERT_FALSE( result.get() );
}
