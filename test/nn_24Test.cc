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
#include <aggregator/nn_24.h>
#include <kvalobs/kvDataOperations.h>

using aggregator::nn_24;

class nn_24Test : public AbstractAggregatorTest
{
protected:
	nn_24Test() :
		factory(1, "2010-03-16 06:00:00", 1)
	{}

	kvalobs::kvDataFactory factory;
	nn_24 aggregator;
};

INSTANTIATE_TEST_CASE_P(nn_24Test, AbstractAggregatorTest, testing::Values(AggregatorPtr(new nn_24)));


TEST_F(nn_24Test, moreThanEnoughData)
{
	kvservice::KvDataList data;
	data.push_back(factory.getData(1, aggregator.readParam(), "2010-03-18 06:00:00"));
	data.push_back(factory.getData(2, aggregator.readParam(), "2010-03-18 12:00:00"));
	data.push_back(factory.getData(3, aggregator.readParam(), "2010-03-18 18:00:00"));
	data.push_back(factory.getData(900, aggregator.readParam(), "2010-03-18 00:00:00"));

	kvalobs::correct(data.front(), 4);

	nn_24::kvDataPtr result = aggregator.process(data.front(), data);

	ASSERT_TRUE( result.get() );

	EXPECT_EQ(2, result->original());
	EXPECT_EQ(3, result->corrected());
}

TEST_F(nn_24Test, tooLittleData)
{
	kvservice::KvDataList data;
	data.push_back(factory.getData(1, aggregator.readParam(), "2010-03-18 06:00:00"));
	data.push_back(factory.getData(3, aggregator.readParam(), "2010-03-18 18:00:00"));

	nn_24::kvDataPtr result = aggregator.process(data.front(), data);

	ASSERT_FALSE( result.get() );
}

TEST_F(nn_24Test, missingPeriod6)
{
	kvservice::KvDataList data;
	data.push_back(factory.getData(1, aggregator.readParam(), "2010-03-18 09:00:00"));
	data.push_back(factory.getData(2, aggregator.readParam(), "2010-03-18 12:00:00"));
	data.push_back(factory.getData(3, aggregator.readParam(), "2010-03-18 18:00:00"));

	nn_24::kvDataPtr result = aggregator.process(data.front(), data);

	ASSERT_FALSE( result.get() );
}

TEST_F(nn_24Test, missingPeriod12)
{
	kvservice::KvDataList data;
	data.push_back(factory.getData(1, aggregator.readParam(), "2010-03-18 06:00:00"));
	data.push_back(factory.getData(2, aggregator.readParam(), "2010-03-18 15:00:00"));
	data.push_back(factory.getData(3, aggregator.readParam(), "2010-03-18 18:00:00"));

	nn_24::kvDataPtr result = aggregator.process(data.front(), data);

	ASSERT_FALSE( result.get() );
}

TEST_F(nn_24Test, missingPeriod18)
{
	kvservice::KvDataList data;
	data.push_back(factory.getData(1, aggregator.readParam(), "2010-03-18 06:00:00"));
	data.push_back(factory.getData(2, aggregator.readParam(), "2010-03-18 12:00:00"));
	data.push_back(factory.getData(3, aggregator.readParam(), "2010-03-18 14:00:00"));

	nn_24::kvDataPtr result = aggregator.process(data.front(), data);

	ASSERT_FALSE( result.get() );
}

TEST_F(nn_24Test, roundDataToOneDecimal) // this is really applicable to all aggregators
{
	kvservice::KvDataList data;
	data.push_back(factory.getData(2, aggregator.readParam(), "2010-03-18 06:00:00"));
	data.push_back(factory.getData(2, aggregator.readParam(), "2010-03-18 12:00:00"));
	data.push_back(factory.getData(3, aggregator.readParam(), "2010-03-18 18:00:00"));

	kvalobs::correct(data.front(), 3);

	nn_24::kvDataPtr result = aggregator.process(data.front(), data);

	ASSERT_TRUE( result.get() );

	// Note that we require exactly equal!
	EXPECT_FLOAT_EQ(2.3, result->original());
	EXPECT_FLOAT_EQ(2.7, result->corrected());
}

TEST_F(nn_24Test, roundDataDownwardsToOneDecimalWhenNegative) // this is really applicable to all aggregators
{
	kvservice::KvDataList data;
	data.push_back(factory.getData(-2, aggregator.readParam(), "2010-03-18 06:00:00"));
	data.push_back(factory.getData(-2, aggregator.readParam(), "2010-03-18 12:00:00"));
	data.push_back(factory.getData(-3, aggregator.readParam(), "2010-03-18 18:00:00"));

	kvalobs::correct(data.front(), -3);

	nn_24::kvDataPtr result = aggregator.process(data.front(), data);

	ASSERT_TRUE( result.get() );

	// Note that we require exactly equal!
	EXPECT_FLOAT_EQ(-2.3, result->original());
	EXPECT_FLOAT_EQ(-2.7, result->corrected());
}
