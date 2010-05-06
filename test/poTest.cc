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
#include <aggregator/po.h>
#include <paramID.h>
#include <kvalobs/kvDataOperations.h>

using namespace aggregator;

class TestingPo : public po
{
protected:
	virtual float getStationMetadata(const std::string & metadataName, const kvalobs::kvData & validFor) const
	{
		if ( metadataName == "VS" )
		{
			if ( validFor.paramID() == UM_VS )
				return 80;
			if ( validFor.paramID() == TM_VS )
				return 2.4;
		}
		if ( metadataName == "hp" )
			return 42;

		throw std::exception();
	}
} p;


TEST(poTest, test)
{
	kvalobs::kvDataFactory factory(42, "2010-05-06 09:00:00", 1);
	const kvalobs::kvData pr = factory.getData(1001.1, PR);
	const kvalobs::kvData ta = factory.getData(12.6, TA);

	po::ParameterSortedDataList toProcess;
	toProcess[PR].push_back(pr);
	toProcess[TA].push_back(ta);

	po::kvDataPtr result = p.process(pr, toProcess);

	// just made up some values
	EXPECT_EQ(36, result->original());
	EXPECT_EQ(35, result->corrected());
}

TEST(poTest, noPr)
{
	kvalobs::kvDataFactory factory(42, "2010-05-06 09:00:00", 1);
	const kvalobs::kvData ta = factory.getData(12.6, TA);

	po::ParameterSortedDataList toProcess;
	toProcess[TA].push_back(ta);

	po::kvDataPtr result = p.process(ta, toProcess);

	EXPECT_FALSE(result);
}

TEST(poTest, noTa)
{
	kvalobs::kvDataFactory factory(42, "2010-05-06 09:00:00", 1);
	const kvalobs::kvData pr = factory.getData(1001.1, PR);

	po::ParameterSortedDataList toProcess;
	toProcess[PR].push_back(pr);

	po::kvDataPtr result = p.process(pr, toProcess);

	EXPECT_FALSE(result);
}

TEST(poTest, missingPr)
{
	kvalobs::kvDataFactory factory(42, "2010-05-06 09:00:00", 1);
	const kvalobs::kvData pr = factory.getMissing(PR);
	const kvalobs::kvData ta = factory.getData(12.6, TA);

	po::ParameterSortedDataList toProcess;
	toProcess[PR].push_back(pr);
	toProcess[TA].push_back(ta);

	po::kvDataPtr result = p.process(pr, toProcess);

	ASSERT_TRUE(result);
	EXPECT_TRUE(kvalobs::missing(* result));
}

TEST(poTest, missingta)
{
	kvalobs::kvDataFactory factory(42, "2010-05-06 09:00:00", 1);
	const kvalobs::kvData pr = factory.getData(1001.1, PR);
	const kvalobs::kvData ta = factory.getMissing(TA);

	po::ParameterSortedDataList toProcess;
	toProcess[PR].push_back(pr);
	toProcess[TA].push_back(ta);

	po::kvDataPtr result = p.process(pr, toProcess);

	ASSERT_TRUE(result);
	EXPECT_TRUE(kvalobs::missing(* result));
}
