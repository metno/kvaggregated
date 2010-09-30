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
#include <proxy/KvalobsProxy.h>
#include <kvalobs/kvDataOperations.h>
#include <kvcpp/mock/FakeKvApp.h>
#include <map>

using namespace aggregator;

class TestingPo : public po
{
	static kvservice::proxy::KvalobsProxy & getProxy()
	{
		static kvservice::proxy::KvalobsProxy * kvalobsProxy = 0;
		if ( ! kvalobsProxy )
			kvalobsProxy = new kvservice::proxy::KvalobsProxy(":memory:");
		return * kvalobsProxy;
	}
public:
	TestingPo() : po(getProxy()), hp(408), um_vs(70), tm_vs(5.7)//, reportedPo(-1)//, previouslyCalculatedPo(-1)
	{
	}

	float hp;
	float um_vs;
	float tm_vs;


protected:
	virtual float getStationMetadata(const std::string & metadataName, const kvalobs::kvData & validFor) const
	{
		if ( metadataName == "VS" )
		{
			if ( validFor.paramID() == UM_VS )
				return um_vs;
			if ( validFor.paramID() == TM_VS )
				return tm_vs;
		}
		if ( metadataName == "hp" )
			return hp;

		throw std::exception();
	}
};

class poTest : public testing::Test
{
public:
	poTest()
	{
	}
protected:
	testing::FakeKvApp app;
	TestingPo p;
};


TEST_F(poTest, createsNegativeTypeId)
{
	kvalobs::kvDataFactory factory(42, "2010-05-06 09:00:00", 1);
	const kvalobs::kvData pr = factory.getData(1036.5, PR);
	const kvalobs::kvData ta = factory.getData(6.3, TA);

	po::ParameterSortedDataList toProcess;
	toProcess[PR].push_back(pr);
	toProcess[TA].push_back(ta);

	po::kvDataPtr result = p.process(pr, toProcess, AbstractAggregator::ParameterSortedDataList());
	ASSERT_TRUE(result);
	EXPECT_EQ(-1, result->typeID());
}

TEST_F(poTest, createsNegativeTypeIdFromNegativeStart)
{
	kvalobs::kvDataFactory factory(42, "2010-05-06 09:00:00", -1);
	const kvalobs::kvData pr = factory.getData(1036.5, PR);
	const kvalobs::kvData ta = factory.getData(6.3, TA);

	po::ParameterSortedDataList toProcess;
	toProcess[PR].push_back(pr);
	toProcess[TA].push_back(ta);

	po::kvDataPtr result = p.process(pr, toProcess, AbstractAggregator::ParameterSortedDataList());
	ASSERT_TRUE(result);
	EXPECT_EQ(-1, result->typeID());
}

TEST_F(poTest, test1)
{
	kvalobs::kvDataFactory factory(42, "2010-05-06 09:00:00", 1);
	const kvalobs::kvData pr = factory.getData(1036.5, PR);
	const kvalobs::kvData ta = factory.getData(6.3, TA);

	po::ParameterSortedDataList toProcess;
	toProcess[PR].push_back(pr);
	toProcess[TA].push_back(ta);

	po::kvDataPtr result = p.process(pr, toProcess, AbstractAggregator::ParameterSortedDataList());

	EXPECT_NEAR(986.5, result->original(), 0.05);
	EXPECT_NEAR(986.5, result->corrected(), 0.05);
}

TEST_F(poTest, test2a)
{
	kvalobs::kvDataFactory factory(42, "2010-05-06 09:00:00", 1);
	const kvalobs::kvData pr = factory.getData(1038.9, PR);
	const kvalobs::kvData ta = factory.getData(-6.3, TA);

	po::ParameterSortedDataList toProcess;
	toProcess[PR].push_back(pr);
	toProcess[TA].push_back(ta);

	po::kvDataPtr result = p.process(pr, toProcess, AbstractAggregator::ParameterSortedDataList());

	EXPECT_NEAR(986.5, result->original(), 0.05);
	EXPECT_NEAR(986.5, result->corrected(), 0.05);
}

TEST_F(poTest, test2b)
{
	kvalobs::kvDataFactory factory(42, "2010-05-06 09:00:00", 1);
	const kvalobs::kvData pr = factory.getData(1037.9, PR); // modified
	const kvalobs::kvData ta = factory.getData(-6.3, TA);

	po::ParameterSortedDataList toProcess;
	toProcess[PR].push_back(pr);
	toProcess[TA].push_back(ta);

	po::kvDataPtr result = p.processMethod2(pr, toProcess);

	EXPECT_NEAR(986.5, result->original(), 0.05);
	EXPECT_NEAR(986.5, result->corrected(), 0.05);
}

TEST_F(poTest, test3)
{
	p.hp = 208;

	kvalobs::kvDataFactory factory(42, "2010-05-06 09:00:00", 1);
	const kvalobs::kvData pr = factory.getData(1011.8, PR);
	const kvalobs::kvData ta = factory.getData(6.3, TA);

	po::ParameterSortedDataList toProcess;
	toProcess[PR].push_back(pr);
	toProcess[TA].push_back(ta);

	po::kvDataPtr result = p.process(pr, toProcess, AbstractAggregator::ParameterSortedDataList());

	EXPECT_NEAR(986.5, result->original(), 0.05);
	EXPECT_NEAR(986.5, result->corrected(), 0.05);
}

TEST_F(poTest, test4a)
{
	p.hp = 208;

	kvalobs::kvDataFactory factory(42, "2010-05-06 09:00:00", 1);
	const kvalobs::kvData pr = factory.getData(1013.0, PR);
	const kvalobs::kvData ta = factory.getData(-6.3, TA);

	po::ParameterSortedDataList toProcess;
	toProcess[PR].push_back(pr);
	toProcess[TA].push_back(ta);

	po::kvDataPtr result = p.process(pr, toProcess, AbstractAggregator::ParameterSortedDataList());

	EXPECT_NEAR(986.5, result->original(), 0.05);
	EXPECT_NEAR(986.5, result->corrected(), 0.05);
}

TEST_F(poTest, test4b)
{
	p.hp = 208;

	kvalobs::kvDataFactory factory(42, "2010-05-06 09:00:00", 1);
	const kvalobs::kvData pr = factory.getData(1012.4, PR);
	const kvalobs::kvData ta = factory.getData(-6.3, TA);

	po::ParameterSortedDataList toProcess;
	toProcess[PR].push_back(pr);
	toProcess[TA].push_back(ta);

	po::kvDataPtr result = p.processMethod2(pr, toProcess);

	EXPECT_NEAR(986.5, result->original(), 0.05);
	EXPECT_NEAR(986.5, result->corrected(), 0.05);
}

TEST_F(poTest, noPr)
{
	kvalobs::kvDataFactory factory(42, "2010-05-06 09:00:00", 1);
	const kvalobs::kvData ta = factory.getData(12.6, TA);

	po::ParameterSortedDataList toProcess;
	toProcess[TA].push_back(ta);

	po::kvDataPtr result = p.process(ta, toProcess, AbstractAggregator::ParameterSortedDataList());

	EXPECT_FALSE(result);
}

TEST_F(poTest, noTa)
{
	kvalobs::kvDataFactory factory(42, "2010-05-06 09:00:00", 1);
	const kvalobs::kvData pr = factory.getData(1001.1, PR);

	po::ParameterSortedDataList toProcess;
	toProcess[PR].push_back(pr);

	po::kvDataPtr result = p.process(pr, toProcess, AbstractAggregator::ParameterSortedDataList());

	EXPECT_FALSE(result);
}

TEST_F(poTest, missingPr)
{
	kvalobs::kvDataFactory factory(42, "2010-05-06 09:00:00", 1);
	const kvalobs::kvData pr = factory.getMissing(PR);
	const kvalobs::kvData ta = factory.getData(12.6, TA);

	po::ParameterSortedDataList toProcess;
	toProcess[PR].push_back(pr);
	toProcess[TA].push_back(ta);

	po::kvDataPtr result = p.process(pr, toProcess, AbstractAggregator::ParameterSortedDataList());

	ASSERT_TRUE(result);
	EXPECT_TRUE(kvalobs::missing(* result));
}

TEST_F(poTest, missingta)
{
	kvalobs::kvDataFactory factory(42, "2010-05-06 09:00:00", 1);
	const kvalobs::kvData pr = factory.getData(1001.1, PR);
	const kvalobs::kvData ta = factory.getMissing(TA);

	po::ParameterSortedDataList toProcess;
	toProcess[PR].push_back(pr);
	toProcess[TA].push_back(ta);

	po::kvDataPtr result = p.process(pr, toProcess, AbstractAggregator::ParameterSortedDataList());

	ASSERT_TRUE(result);
	EXPECT_TRUE(kvalobs::missing(* result));
}


TEST_F(poTest, testCorrectedValuesSetsFmis)
{
	p.hp = 208;

	kvalobs::kvDataFactory factory(42, "2010-05-06 09:00:00", 1);
	const kvalobs::kvData pr = factory.getData(1011.8, PR);
	kvalobs::kvData ta = factory.getData(6.3, TA);
	kvalobs::correct(ta, 23.2);

	po::ParameterSortedDataList toProcess;
	toProcess[PR].push_back(pr);
	toProcess[TA].push_back(ta);

	po::kvDataPtr result = p.process(pr, toProcess, AbstractAggregator::ParameterSortedDataList());

	EXPECT_NEAR(986.5, result->original(), 0.05);
	//EXPECT_NEAR(986.5, result->corrected(), 0.05); // We don't care
	EXPECT_EQ(4, result->controlinfo().flag(kvalobs::flag::fmis)) << "Error in kvalobs version";
}

TEST_F(poTest, testMinorCorrectionsAreNotWritten)
{
	kvalobs::kvDataFactory factory(42, "2010-05-06 09:00:00", 1);
	const kvalobs::kvData pr = factory.getData(1036.5, PR);
	const kvalobs::kvData ta = factory.getData(6.3, TA);
	const kvalobs::kvData po = factory.getData(986.21, PO);

	po::ParameterSortedDataList toProcess;
	toProcess[PR].push_back(pr);
	toProcess[TA].push_back(ta);
	toProcess[PO].push_back(po);

	po::kvDataPtr result = p.process(pr, toProcess, AbstractAggregator::ParameterSortedDataList());
	// calculation should give the value 986.5.
	// but since the value of original PO observation is very close, we expect no return
	EXPECT_TRUE(! result);
}

TEST_F(poTest, testMinorCorrectionsAreNotWritten2)
{
	kvalobs::kvDataFactory factory(42, "2010-05-06 09:00:00", 1);
	const kvalobs::kvData pr = factory.getData(1036.5, PR);
	const kvalobs::kvData ta = factory.getData(6.3, TA);
	const kvalobs::kvData po = factory.getData(986.79, PO);

	po::ParameterSortedDataList toProcess;
	toProcess[PR].push_back(pr);
	toProcess[TA].push_back(ta);
	toProcess[PO].push_back(po);

	po::kvDataPtr result = p.process(pr, toProcess, AbstractAggregator::ParameterSortedDataList());
	// calculation should give the value 986.5.
	// but since the value of original PO observation is very close, we expect no return
	EXPECT_TRUE(! result);
}

TEST_F(poTest, testMajorCorrectionsAreWritten)
{
	kvalobs::kvDataFactory factory(42, "2010-05-06 09:00:00", 1);
	const kvalobs::kvData pr = factory.getData(1036.5, PR);
	const kvalobs::kvData ta = factory.getData(6.3, TA);
	const kvalobs::kvData po = factory.getData(987, PO);

	po::ParameterSortedDataList toProcess;
	toProcess[PR].push_back(pr);
	toProcess[TA].push_back(ta);
	toProcess[PO].push_back(po);

	po::kvDataPtr result = p.process(pr, toProcess, AbstractAggregator::ParameterSortedDataList());
	ASSERT_TRUE(result);
	EXPECT_NEAR(986.5, result->original(), 0.05);
}

TEST_F(poTest, testMinorCorrectionsAreWrittenIfOldDataExists)
{
	kvalobs::kvDataFactory factory(42, "2010-05-06 09:00:00", 1);
	const kvalobs::kvData pr = factory.getData(1036.5, PR);
	const kvalobs::kvData ta = factory.getData(6.3, TA);
	const kvalobs::kvData po = factory.getData(986.21, PO);

	po::ParameterSortedDataList toProcess;
	toProcess[PR].push_back(pr);
	toProcess[TA].push_back(ta);
	toProcess[PO].push_back(po);

	AbstractAggregator::ParameterSortedDataList aggregated;
	kvalobs::kvDataFactory agFactory(42, "2010-05-06 09:00:00", -1);
	aggregated[PO].push_back(agFactory.getData(986.21, PO));

	po::kvDataPtr result = p.process(pr, toProcess, aggregated);
	// calculation should give the value 986.5.
	// normally, we would not expect a return, since almost the same data have
	// been reported by the station. But since data have already been stored
	// in database, we must send a correction.
	ASSERT_TRUE(result);
	EXPECT_NEAR(986.5, result->original(), 0.05);
}

TEST_F(poTest, testOriginalDifferenceCauseAggregation)
{
	kvalobs::kvDataFactory factory(42, "2010-05-06 09:00:00", 1);
	const kvalobs::kvData pr = factory.getData(1036.5, PR);
	const kvalobs::kvData ta = factory.getData(6.3, TA);
	const kvalobs::kvData po = factory.getData(987, PO);

	po::ParameterSortedDataList toProcess;
	toProcess[PR].push_back(pr);
	toProcess[TA].push_back(ta);
	toProcess[PO].push_back(po);

	po::kvDataPtr result = p.process(pr, toProcess, AbstractAggregator::ParameterSortedDataList());
	ASSERT_TRUE(result);
	EXPECT_NEAR(986.5, result->original(), 0.05);
}

TEST_F(poTest, testCorrectedDifferenceCauseAggregation)
{
	kvalobs::kvDataFactory factory(42, "2010-05-06 09:00:00", 1);
	const kvalobs::kvData pr = factory.getData(1036.5, PR);
	const kvalobs::kvData ta = factory.getData(6.3, TA);
	kvalobs::kvData po = factory.getData(986.5, PO);
	kvalobs::correct(po, 980);

	po::ParameterSortedDataList toProcess;
	toProcess[PR].push_back(pr);
	toProcess[TA].push_back(ta);
	toProcess[PO].push_back(po);

	po::kvDataPtr result = p.process(pr, toProcess, AbstractAggregator::ParameterSortedDataList());
	ASSERT_TRUE(result);
	EXPECT_NEAR(986.5, result->original(), 0.05);
}
