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
#include <checkDecision/CompleteCheckDecider.h>
#include <paramID.h>
#include <kvalobs/kvDataOperations.h>


using namespace aggregator;

namespace
{
std::string dummy;
}

TEST(ForeignStationPrecipitationFilterCheck, skipForeignStationWithoutPrecipitation)
{
	const kvalobs::kvDataFactory foreign(102313, "2010-09-15 06:00:00", 302);

	CompleteCheckDecider filter;
	ASSERT_FALSE(
			filter.shouldRunChecksOn(foreign.getData(0, 12),
					CompleteCheckDecider::DataList(), dummy));
}

TEST(ForeignStationPrecipitationFilterCheck, checkForeignStationWithPrecipitation)
{
	const kvalobs::kvDataFactory foreign(102313, "2010-09-15 06:00:00", 302);

	CompleteCheckDecider filter;
	EXPECT_TRUE(
			filter.shouldRunChecksOn(foreign.getData(0, 109),
					CompleteCheckDecider::DataList(), dummy));
}

TEST(ForeignStationPrecipitationFilterCheck, checkRegularStationWithPrecipitation)
{
	const kvalobs::kvDataFactory foreign(1023, "2010-09-15 06:00:00", 302);

	CompleteCheckDecider filter;
	EXPECT_TRUE(
			filter.shouldRunChecksOn(foreign.getData(0, 28),
					CompleteCheckDecider::DataList(), dummy));
}

TEST(RaOverridesRr1Check, stationWithRr1ButNotRa)
{
	const kvalobs::kvDataFactory factory(1023, "2010-09-15 06:00:00", 302);
	CompleteCheckDecider::DataList data;
	data.push_back(factory.getData(0, RR_1));

	CompleteCheckDecider filter;
	EXPECT_TRUE(filter.shouldRunChecksOn(data.front(), data, dummy));
}

TEST(RaOverridesRr1Check, stationWithRaButNotRr1)
{
	const kvalobs::kvDataFactory factory(1023, "2010-09-15 06:00:00", 302);
	CompleteCheckDecider::DataList data;
	data.push_back(factory.getData(0, RA));

	CompleteCheckDecider filter;
	EXPECT_TRUE(filter.shouldRunChecksOn(data.front(), data, dummy));
}

TEST(RaOverridesRr1Check, stationWithRaAndRr1)
{
	const kvalobs::kvDataFactory factory(1023, "2010-09-15 06:00:00", 302);
	CompleteCheckDecider::DataList data;
	data.push_back(factory.getData(0, RA));
	data.push_back(factory.getData(0, RR_1));

	CompleteCheckDecider filter;
	EXPECT_FALSE(filter.shouldRunChecksOn(data.front(), data, dummy));
	EXPECT_TRUE(filter.shouldRunChecksOn(data.back(), data, dummy));
}
