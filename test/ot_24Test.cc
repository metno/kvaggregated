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

#include <gtest/gtest.h>
#include <aggregator/ot_24.h>
#include <paramID.h>
#include <kvalobs/kvDataOperations.h>
#include <iterator>

namespace
{
boost::posix_time::ptime pt(const std::string & s)
{
	return boost::posix_time::time_from_string(s);
}
}


using namespace aggregator;

TEST(ot_24Test, sumsCorrectly)
{
	ot_24 aggregator;

	kvalobs::kvDataFactory f(100, pt("2010-12-16 00:00:00"), 302);
	ot_24::ParameterSortedDataList data;
	ot_24::kvDataList & d = data[OT_1];
	for ( boost::posix_time::ptime t = pt("2010-12-15 01:00:00"); t <= f.obstime(); t += boost::posix_time::hours(1) )
		d.push_back(f.getData(30, OT_1, t));

	ot_24::kvDataPtr result = aggregator.process(d.back(), data);

	ASSERT_TRUE(result);
	EXPECT_EQ(30*24, result->original());
	EXPECT_EQ(30*24, result->corrected());
}

TEST(ot_24Test, failOnNegativeOriginal)
{
	ot_24 aggregator;

	kvalobs::kvDataFactory f(100, pt("2010-12-16 00:00:00"), 302);
	ot_24::ParameterSortedDataList data;
	ot_24::kvDataList & d = data[OT_1];
	for ( boost::posix_time::ptime t = pt("2010-12-15 01:00:00"); t <= f.obstime(); t += boost::posix_time::hours(1) )
		d.push_back(f.getData(30, OT_1, t));

	ot_24::kvDataList::iterator d3 = d.begin();
	std::advance(d3, 3);
	* d3 = f.getData(-1, OT_1, pt("2010-12-15 04:00:00"));
	kvalobs::correct(* d3, 30);

	ot_24::kvDataPtr result = aggregator.process(d.back(), data);

	ASSERT_TRUE(result);
	EXPECT_EQ((30*23) -1, result->original());
	EXPECT_EQ(30*24, result->corrected());
}

TEST(ot_24Test, failOnNegativeCorrected)
{
	ot_24 aggregator;

	kvalobs::kvDataFactory f(100, pt("2010-12-16 00:00:00"), 302);
	ot_24::ParameterSortedDataList data;
	ot_24::kvDataList & d = data[OT_1];
	for ( boost::posix_time::ptime t = pt("2010-12-15 01:00:00"); t <= f.obstime(); t += boost::posix_time::hours(1) )
		d.push_back(f.getData(30, OT_1, t));

	ot_24::kvDataList::iterator d2 = d.begin();
	std::advance(d2, 3);
	kvalobs::correct(* d2, -8);

	ot_24::kvDataPtr result = aggregator.process(d.back(), data);

	ASSERT_TRUE(result);
	EXPECT_EQ(30*24, result->original());
	EXPECT_EQ((30*23) -8, result->corrected());
}

TEST(ot_24Test, failOnTooHighOriginal)
{
	ot_24 aggregator;

	kvalobs::kvDataFactory f(100, pt("2010-12-16 00:00:00"), 302);
	ot_24::ParameterSortedDataList data;
	ot_24::kvDataList & d = data[OT_1];
	for ( boost::posix_time::ptime t = pt("2010-12-15 01:00:00"); t <= f.obstime(); t += boost::posix_time::hours(1) )
		d.push_back(f.getData(30, OT_1, t));

	ot_24::kvDataList::iterator d3 = d.begin();
	std::advance(d3, 3);
	* d3 = f.getData(61, OT_1, pt("2010-12-15 04:00:00"));
	kvalobs::correct(* d3, 30);

	ot_24::kvDataPtr result = aggregator.process(d.back(), data);

	ASSERT_TRUE(result);
	EXPECT_EQ((30*23) +61, result->original());
	EXPECT_EQ(30*24, result->corrected());
}

TEST(ot_24Test, failOnTooHighCorrected)
{
	ot_24 aggregator;

	kvalobs::kvDataFactory f(100, pt("2010-12-16 00:00:00"), 302);
	ot_24::ParameterSortedDataList data;
	ot_24::kvDataList & d = data[OT_1];
	for ( boost::posix_time::ptime t = pt("2010-12-15 01:00:00"); t <= f.obstime(); t += boost::posix_time::hours(1) )
		d.push_back(f.getData(30, OT_1, t));

	ot_24::kvDataList::iterator d2 = d.begin();
	std::advance(d2, 3);
	kvalobs::correct(* d2, 422);

	ot_24::kvDataPtr result = aggregator.process(d.back(), data);

	ASSERT_TRUE(result);
	EXPECT_EQ(30*24, result->original());
	EXPECT_EQ((30*23) +422, result->corrected());
}

TEST(ot_24Test, correctWorkingWithMinuteData)
{
	ot_24 aggregator;

	kvalobs::kvDataFactory f(100, pt("2010-12-16 00:00:00"), 302);
	ot_24::ParameterSortedDataList data;
	ot_24::kvDataList & d = data[OT_1];
	for ( boost::posix_time::ptime t = pt("2010-12-15 01:00:00"); t <= f.obstime(); t += boost::posix_time::minutes(1) )
		d.push_back(f.getData(30, OT_1, t));

	ot_24::kvDataPtr result = aggregator.process(d.back(), data);


	ASSERT_TRUE(result);
	EXPECT_EQ(30*24, result->original());
	EXPECT_EQ(30*24, result->corrected());
}
