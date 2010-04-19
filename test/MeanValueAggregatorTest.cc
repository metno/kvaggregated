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
#include <paramID.h>
#include <agregator/MeanValueAggregator.h>
#include <kvalobs/kvDataOperations.h>

using aggregator::MeanValueAggregator;

class MeanValueAgregatorTest : public AbstractAggregatorTest
{
protected:
	MeanValueAgregatorTest() :
		factory(1, "2010-03-16 06:00:00", 1),
		agregator(1, 2)
	{}

	kvalobs::kvDataFactory factory;
	MeanValueAggregator agregator;
};

INSTANTIATE_TEST_CASE_P(MeanValueAgregatorTest, AbstractAggregatorTest, testing::Values(AggregatorPtr(new MeanValueAggregator(1,2))));


TEST_F(MeanValueAgregatorTest, standard24hAgregate)
{
	kvservice::KvDataList data;
	int i = 0;
	for ( miutil::miTime t = "2010-03-15 07:00:00"; t <= factory.obstime(); t.addHour() )
		data.push_back(factory.getData(++ i, 1, t));

	MeanValueAggregator::kvDataPtr result = agregator.process(data.front(), data);

	ASSERT_TRUE( result.get() );

	EXPECT_EQ(12.5, result->original());
	EXPECT_EQ(12.5, result->corrected());
}

TEST_F(MeanValueAgregatorTest, missingValue)
{
	kvservice::KvDataList data;
	for ( miutil::miTime t = "2010-03-15 07:00:00"; t <= factory.obstime(); t.addHour() )
		data.push_back(factory.getData(1, 1, t));

	kvalobs::reject(data.front());

	MeanValueAggregator::kvDataPtr result = agregator.process(data.front(), data);

	ASSERT_TRUE( result.get() );

	EXPECT_EQ(1, result->original());
	EXPECT_FALSE(kvalobs::valid(* result));
}

TEST_F(MeanValueAgregatorTest, observationEach3Hours)
{
	kvservice::KvDataList data;
	int i = 0;
	for ( miutil::miTime t = "2010-03-15 09:00:00"; t <= factory.obstime(); t.addHour(3) )
		data.push_back(factory.getData(++ i, 1, t));

	kvalobs::reject(data.front());

	MeanValueAggregator::kvDataPtr result = agregator.process(data.front(), data);

	ASSERT_TRUE( result.get() );

	EXPECT_EQ(4.5, result->original());
	EXPECT_FALSE(kvalobs::valid(* result));
}
