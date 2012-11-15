/*
  Kvalobs - Free Quality Control Software for Meteorological Observations 

  $Id: ra2rr_12ForwardTest.cc,v 1.1.2.3 2007/09/27 09:02:16 paule Exp $                                                       

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

#include "AbstractAggregatorTest.h"
#include <aggregator/ra2rr_12_forward.h>
#include <kvalobs/kvDataOperations.h>

using namespace kvalobs;
using namespace aggregator;


class ra2rr_12ForwardTest : public AbstractAggregatorTest
{
protected:
    enum { RR_12 = 109, RA = 104 };
	ra2rr_12_forward aggregator;
};

namespace
{
boost::posix_time::ptime pt(const std::string & s)
{
	return boost::posix_time::time_from_string(s);
}
}

// This class does not follow the default time span of most other tests
//INSTANTIATE_TEST_CASE_P(ra2rr_12ForwardTest, AbstractAgregatorTest, testing::Values(AggregatorPtr(new ra2rr_12_forward)));

TEST_F(ra2rr_12ForwardTest, testGetTimeSpanAtGenerationPoint)
{
	const kvalobs::kvDataFactory dataFactory( 42, pt("2007-06-06 06:00:00"), 302 );
	const StandardAggregator::TimeSpan timeSpan = 
	aggregator.getTimeSpan(dataFactory.getData( 15, 1 ));
	EXPECT_EQ(pt("2007-06-05 18:00:00"), timeSpan.first );
	EXPECT_EQ(pt("2007-06-06 18:00:00"), timeSpan.second );
}


TEST_F(ra2rr_12ForwardTest, testGetTimeSpan)
{
	const kvalobs::kvDataFactory dataFactory( 42, pt("2007-06-06 03:00:00"), 302 );
	const StandardAggregator::TimeSpan timeSpan = 
	aggregator.getTimeSpan(dataFactory.getData( 15, 1 ));
	EXPECT_EQ(pt("2007-06-05 18:00:00"), timeSpan.first );
	EXPECT_EQ(pt("2007-06-06 18:00:00"), timeSpan.second );
}

TEST_F(ra2rr_12ForwardTest, testExpressedInterest)
{
    const boost::gregorian::date d = boost::gregorian::day_clock::universal_day();
    boost::posix_time::ptime t( d, boost::posix_time::time_duration( 6, 0, 0 ) );

    const kvDataFactory dataFactory( 42, t, 302 );

    ASSERT_TRUE( aggregator.isInterestedIn( dataFactory.getData( 0, RA ) ) );

    // TODO: complete this test:
    //	t = miutil::miTime::nowTime();
    //	t.addHour( 1 );
    //	ASSERT_TRUE( not aggregator.isInterestedIn( dataFactory.getData( 0, RA, t ) ) );
}

TEST_F(ra2rr_12ForwardTest, testNotEnoughData)
{
	AbstractAggregator::ParameterSortedDataList data;
	StandardAggregator::kvDataList & dl = data[RA];
    const kvDataFactory dataFactory( 42, pt("2007-06-06 06:00:00"), 302 );
    dl.push_back( dataFactory.getData( 213.3, RA ) );

    StandardAggregator::kvDataPtr d = aggregator.process( dl.front(), data );
    ASSERT_TRUE( not d.get() );
}

TEST_F(ra2rr_12ForwardTest, testDataMarkedAsMissing)
{
	AbstractAggregator::ParameterSortedDataList data;
	StandardAggregator::kvDataList & dl = data[RA];
    const kvDataFactory dataFactory( 42, pt("2007-06-06 06:00:00"), 302 );
    dl.push_back( dataFactory.getData( 210.1, RA, pt("2007-06-05 06:00:00") ) );
    dl.push_back( dataFactory.getMissing( RA, pt("2007-06-05 18:00:00") ) );
    dl.push_back( dataFactory.getData( 213.3, RA, pt("2007-06-06 06:00:00") ) );

    StandardAggregator::kvDataPtr d = aggregator.process( dl.front(), data );
    ASSERT_TRUE( d.get() );
    ASSERT_TRUE( missing( * d ) );
}

TEST_F(ra2rr_12ForwardTest, test12hZero)
{
	AbstractAggregator::ParameterSortedDataList data;
	StandardAggregator::kvDataList & dl = data[RA];
    const kvDataFactory dataFactory( 42, pt("2007-06-06 06:00:00"), 302 );
    dl.push_back( dataFactory.getData( 210.1, RA, pt("2007-06-05 06:00:00") ) );
    dl.push_back( dataFactory.getData( 210.1, RA, pt("2007-06-05 18:00:00") ) );
    dl.push_back( dataFactory.getData( 209.0, RA, pt("2007-06-04 18:00:00") ) ); // note obstime earliest

    StandardAggregator::kvDataPtr d = aggregator.process( dl.front(), data );
    ASSERT_TRUE( d.get() );
    EXPECT_EQ( pt("2007-06-05 18:00:00"), d->obstime() );
    EXPECT_FLOAT_EQ( 0, d->corrected() );
}


TEST_F(ra2rr_12ForwardTest, test12hNegative)
{
	AbstractAggregator::ParameterSortedDataList data;
	StandardAggregator::kvDataList & dl = data[RA];
    const kvDataFactory dataFactory( 42, pt("2007-06-06 06:00:00"), 302 );
    dl.push_back( dataFactory.getData( 210.1, RA, pt("2007-06-05 06:00:00") ) );
    dl.push_back( dataFactory.getData( 209.0, RA, pt("2007-06-05 18:00:00") ) );
    dl.push_back( dataFactory.getData( 209.0, RA, pt("2007-06-04 18:00:00") ) ); // note obstime earliest

    StandardAggregator::kvDataPtr d = aggregator.process( dl.front(), data );
    ASSERT_TRUE( d.get() );
    EXPECT_EQ( pt("2007-06-05 18:00:00"), d->obstime() );
    EXPECT_FLOAT_EQ( 0, d->corrected() );
}


TEST_F(ra2rr_12ForwardTest, test12hPositive24hNegative)
{
	AbstractAggregator::ParameterSortedDataList data;
	StandardAggregator::kvDataList & dl = data[RA];
    const kvDataFactory dataFactory( 42, pt("2007-06-06 06:00:00"), 302 );
    dl.push_back( dataFactory.getData( 210.1, RA, pt("2007-06-05 06:00:00") ) );
    dl.push_back( dataFactory.getData( 211.2, RA, pt("2007-06-05 18:00:00") ) );
    dl.push_back( dataFactory.getData( 214.4, RA, pt("2007-06-04 18:00:00") ) ); // note obstime earliest

    StandardAggregator::kvDataPtr d = aggregator.process( dl.front(), data );
    ASSERT_TRUE( d.get() );
    EXPECT_EQ( pt("2007-06-05 18:00:00"), d->obstime() );
    EXPECT_FLOAT_EQ( 0, d->corrected() );
}


TEST_F(ra2rr_12ForwardTest, test12hPositive24hZero)
{
	AbstractAggregator::ParameterSortedDataList data;
	StandardAggregator::kvDataList & dl = data[RA];
    const kvDataFactory dataFactory( 42, pt("2007-06-06 06:00:00"), 302 );
    dl.push_back( dataFactory.getData( 210.1, RA, pt("2007-06-05 06:00:00") ) );
    dl.push_back( dataFactory.getData( 211.2, RA, pt("2007-06-05 18:00:00") ) );
    dl.push_back( dataFactory.getData( 211.2, RA, pt("2007-06-04 18:00:00") ) ); // note obstime earliest

    StandardAggregator::kvDataPtr d = aggregator.process( dl.front(), data );
    ASSERT_TRUE( d.get() );
    EXPECT_EQ( pt("2007-06-05 18:00:00"), d->obstime() );
    EXPECT_FLOAT_EQ( 0, d->corrected() );
}


TEST_F(ra2rr_12ForwardTest, test12hPositive24hPositivePrev12hNegative)
{
	AbstractAggregator::ParameterSortedDataList data;
	StandardAggregator::kvDataList & dl = data[RA];
    const kvDataFactory dataFactory( 42, pt("2007-06-06 06:00:00"), 302 );
    dl.push_back( dataFactory.getData( 209.0, RA, pt("2007-06-05 06:00:00") ) );
    dl.push_back( dataFactory.getData( 211.2, RA, pt("2007-06-05 18:00:00") ) );
    dl.push_back( dataFactory.getData( 210.1, RA, pt("2007-06-04 18:00:00") ) ); // note obstime earliest

    StandardAggregator::kvDataPtr d = aggregator.process( dl.front(), data );
    ASSERT_TRUE( d.get() );
    EXPECT_EQ( pt("2007-06-05 18:00:00"), d->obstime() );
    EXPECT_NEAR( 1.1, d->corrected(), .00001 );
}
		

TEST_F(ra2rr_12ForwardTest, test12hPositive24hPositivePrev12hZero)
{
	AbstractAggregator::ParameterSortedDataList data;
	StandardAggregator::kvDataList & dl = data[RA];
    const kvDataFactory dataFactory( 42, pt("2007-06-06 06:00:00"), 302 );
    dl.push_back( dataFactory.getData( 209.0, RA, pt("2007-06-05 06:00:00") ) );
    dl.push_back( dataFactory.getData( 211.2, RA, pt("2007-06-05 18:00:00") ) );
    dl.push_back( dataFactory.getData( 211.0, RA, pt("2007-06-04 18:00:00") ) ); // note obstime earliest

    StandardAggregator::kvDataPtr d = aggregator.process( dl.front(), data );
    ASSERT_TRUE( d.get() );
    EXPECT_EQ( pt("2007-06-05 18:00:00"), d->obstime() );
    EXPECT_NEAR( 0.2, d->corrected(), .00001 );
}


TEST_F(ra2rr_12ForwardTest, test12hPositive24hPositivePrev12hPositive)
{
	AbstractAggregator::ParameterSortedDataList data;
	StandardAggregator::kvDataList & dl = data[RA];
    const kvDataFactory dataFactory( 42, pt("2007-06-06 06:00:00"), 302 );
    dl.push_back( dataFactory.getData( 210.1, RA, pt("2007-06-05 06:00:00") ) );
    dl.push_back( dataFactory.getData( 211.2, RA, pt("2007-06-05 18:00:00") ) );
    dl.push_back( dataFactory.getData( 209.0, RA, pt("2007-06-04 18:00:00") ) ); // note obstime earliest

    StandardAggregator::kvDataPtr d = aggregator.process( dl.front(), data );
    ASSERT_TRUE( d.get() );
    EXPECT_EQ( pt("2007-06-05 18:00:00"), d->obstime() );
    EXPECT_NEAR( 1.1, d->corrected(), .00001 );
}


