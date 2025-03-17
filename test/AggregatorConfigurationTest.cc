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
#include <configuration/AggregatorConfiguration.h>
#include <sstream>

class AggregatorConfigurationTest : public testing::Test
{
protected:
	AggregatorConfiguration config;
	std::ostringstream msg;
	std::ostringstream err;

	AggregatorConfigurationTest() :
		config(msg, err)
	{}

	AggregatorConfiguration::ParseResult parse(int & argc, const char ** argv)
	{
		return config.parse(argc, const_cast<char **>(argv));
	}
};

TEST_F(AggregatorConfigurationTest, helpString)
{
	int argc = 2;
	const char * argv[] = {"test", "--help"};

	EXPECT_EQ(AggregatorConfiguration::Exit_Success, parse(argc, argv));

	EXPECT_TRUE(not msg.str().empty());
	EXPECT_TRUE(err.str().empty());
}


TEST_F(AggregatorConfigurationTest, stationListSingle)
{
	int argc = 2;
	const char * argv[] = {"test", "-s1"};

	EXPECT_EQ(AggregatorConfiguration::No_Action, parse(argc, argv));

	const std::vector<int> & stations = config.stations();
	ASSERT_FALSE(stations.empty());

	EXPECT_EQ(1u, stations.size()) << "(last value is " << stations.back() << ")";

	EXPECT_EQ(1, stations[0]);
}


TEST_F(AggregatorConfigurationTest, stationList)
{
	int argc = 2;
	const char * argv[] = {"test", "-s1,2,3"};

	EXPECT_EQ(AggregatorConfiguration::No_Action, parse(argc, argv));

	const std::vector<int> & stations = config.stations();
	ASSERT_FALSE(stations.empty());

	EXPECT_EQ(3u, stations.size()) << "(last value is " << stations.back() << ")";

	for (int i = 0; i < 3; ++ i )
		EXPECT_EQ(i +1, stations[i]);
}

TEST_F(AggregatorConfigurationTest, stationListManyTimes)
{
	int argc = 3;
	const char * argv[] = {"test", "-s1,2,3", "-s4"};

	EXPECT_EQ(AggregatorConfiguration::No_Action, parse(argc, argv));

	const std::vector<int> & stations = config.stations();
	ASSERT_FALSE(stations.empty());

	EXPECT_EQ(4u, stations.size()) << "(last value is " << stations.back() << ")";

	for (int i = 0; i < 4; ++ i )
		EXPECT_EQ(i +1, stations[i]);
}
