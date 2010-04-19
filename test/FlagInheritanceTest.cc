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
#include <agregator/useinfoAggregate.h>
#include <times.h>
#include <kvalobs/kvDataOperations.h>

TEST(FlagInheritanceTest, AllValuesUncheckedUnchecked )
{
	aggregator::UseList l;
	for ( int i = 0; i < 6; ++ i)
		l.push_back("99999");

	aggregator::BaseUseInfo result = aggregate(l);

	EXPECT_EQ("99999", result);
}

TEST(FlagInheritanceTest, Perfect )
{
	// case 1
	aggregator::UseList l;
	for ( int i = 0; i < 6; ++ i)
		l.push_back("00000");

	aggregator::BaseUseInfo result = aggregate(l);

	EXPECT_EQ("99000", result);
}

TEST(FlagInheritanceTest, SuspiciousUnchanged )
{
	// case 2a
	aggregator::UseList l;
	for ( int i = 0; i < 6; ++ i)
		l.push_back("00000");
	l[3] = "00109";

	aggregator::BaseUseInfo result = aggregate(l);

	EXPECT_EQ("99109", result);
}

TEST(FlagInheritanceTest, SuspiciousUnchanged_inheritHighestUncertainty )
{
	// case 2a
	aggregator::UseList l;
	for ( int i = 0; i < 6; ++ i)
		l.push_back("00000");
	l[1] = "00109";
	l[3] = "00209";
	l[4] = "00109";

	aggregator::BaseUseInfo result = aggregate(l);

	EXPECT_EQ("99209", result);
}

TEST(FlagInheritanceTest, SomeSuspiciousChanged )
{
	// case 2b
	aggregator::UseList l;
	for ( int i = 0; i < 6; ++ i)
		l.push_back("00000");
	l[3] = "00219";
	l[4] = "00109";

	aggregator::BaseUseInfo result = aggregate(l);

	EXPECT_EQ("99109", result);
}

TEST(FlagInheritanceTest, AllSuspiciousManuallyChanged )
{
	// case 2b0
	aggregator::UseList l;
	for ( int i = 0; i < 6; ++ i)
		l.push_back("00000");
	l[3] = "00227";
	l[4] = "00119";

	aggregator::BaseUseInfo result = aggregate(l);

	EXPECT_EQ("99027", result);
}

TEST(FlagInheritanceTest, AllSuspiciousManuallyChangedOneAutomaticallyChanged )
{
	// case 2b1
	aggregator::UseList l;
	for ( int i = 0; i < 6; ++ i)
		l.push_back("00000");
	l[2] = "00217";
	l[4] = "00169";

	aggregator::BaseUseInfo result = aggregate(l);

	EXPECT_EQ("99169", result);
}

TEST(FlagInheritanceTest, OneUncheckedAllOthersGood )
{
	// case 3a
	aggregator::UseList l;
	for ( int i = 0; i < 6; ++ i)
		l.push_back("00000");
	l[2] = "99999";

	aggregator::BaseUseInfo result = aggregate(l);

	EXPECT_EQ("99999", result);
}

TEST(FlagInheritanceTest, OneUncheckedSuspiciousUnchanged )
{
	// case 3b-2a
	aggregator::UseList l;
	for ( int i = 0; i < 6; ++ i)
		l.push_back("00000");
	l[0] = "99999";
	l[3] = "00109";

	aggregator::BaseUseInfo result = aggregate(l);

	EXPECT_EQ("99109", result);
}

TEST(FlagInheritanceTest, OneUncheckedSuspiciousUnchanged_inheritHighestUncertainty )
{
	// case 3b-2a
	aggregator::UseList l;
	for ( int i = 0; i < 6; ++ i)
		l.push_back("00000");
	l[0] = "99999";
	l[1] = "00109";
	l[3] = "00209";
	l[4] = "00109";

	aggregator::BaseUseInfo result = aggregate(l);

	EXPECT_EQ("99209", result);
}

TEST(FlagInheritanceTest, OneUncheckedSomeSuspiciousChanged )
{
	// case 3b-2b
	aggregator::UseList l;
	for ( int i = 0; i < 6; ++ i)
		l.push_back("00000");
	l[0] = "99999";
	l[3] = "00219";
	l[4] = "00109";

	aggregator::BaseUseInfo result = aggregate(l);

	EXPECT_EQ("99109", result);
}

TEST(FlagInheritanceTest, OneUncheckedAllSuspiciousManuallyChanged )
{
	// case 3b-2b0
	aggregator::UseList l;
	for ( int i = 0; i < 6; ++ i)
		l.push_back("00000");
	l[0] = "99999";
	l[3] = "00227";
	l[4] = "00119";

	aggregator::BaseUseInfo result = aggregate(l);

	EXPECT_EQ("99027", result);
}

TEST(FlagInheritanceTest, OneUncheckedAllSuspiciousManuallyChangedOneAutomaticallyChanged )
{
	// case 3b-2b1
	aggregator::UseList l;
	for ( int i = 0; i < 6; ++ i)
		l.push_back("00000");
	l[0] = "99999";
	l[2] = "00217";
	l[4] = "00169";

	aggregator::BaseUseInfo result = aggregate(l);

	EXPECT_EQ("99169", result);
}

TEST(FlagInheritanceTest, OneMissingOriginalAllOtherGood )
{
	// case 4
	aggregator::UseList l;
	for ( int i = 0; i < 6; ++ i)
		l.push_back("00000");
	l[4] = "98999"; // Missing original value

	aggregator::BaseUseInfo result = aggregate(l);

	EXPECT_EQ("98999", result);
}

TEST(FlagInheritanceTest, OneMissingOriginalOneSuspicious )
{
	// case 4
	aggregator::UseList l;
	for ( int i = 0; i < 6; ++ i)
		l.push_back("00000");
	l[4] = "98999"; // Missing original value
	l[3] = "00209";

	aggregator::BaseUseInfo result = aggregate(l);

	EXPECT_EQ("98999", result);
}

TEST(FlagInheritanceTest, OneRejectedAllOtherGood )
{
	// case 5
	aggregator::UseList l;
	for ( int i = 0; i < 6; ++ i)
		l.push_back("00000");
	l[4] = "99389"; // rejected

	aggregator::BaseUseInfo result = aggregate(l);

	EXPECT_EQ("99989", result);
}

TEST(FlagInheritanceTest, OneRejectedAllOtherGoodOneSuspicious )
{
	// case 5
	aggregator::UseList l;
	for ( int i = 0; i < 6; ++ i)
		l.push_back("00000");
	l[4] = "99389"; // rejected
	l[3] = "00209";

	aggregator::BaseUseInfo result = aggregate(l);

	EXPECT_EQ("99989", result);
}

TEST(FlagInheritanceTest, OneRejectedOneMissing )
{
	// case 4/5
	aggregator::UseList l;
	for ( int i = 0; i < 6; ++ i)
		l.push_back("00000");
	l[3] = "99389"; // rejected
	l[4] = "98999"; // Missing original value

	aggregator::BaseUseInfo result = aggregate(l);

	EXPECT_EQ("98999", result);
}

TEST(FlagInheritanceTest, OneManuallyInterpolatedNoSuspicious)
{
	// case 6a
	aggregator::UseList l;
	for ( int i = 0; i < 6; ++ i)
		l.push_back("00000");
	l[2] = "98929";

	aggregator::BaseUseInfo result = aggregate(l);

	EXPECT_EQ("98029", result);
}

TEST(FlagInheritanceTest, OneAutomaticallyInterpolatedNoSuspicious)
{
	// case 6a
	aggregator::UseList l;
	for ( int i = 0; i < 6; ++ i)
		l.push_back("00000");
	l[2] = "98949";

	aggregator::BaseUseInfo result = aggregate(l);

	EXPECT_EQ("98147", result);
}

TEST(FlagInheritanceTest, OneManuallyCorrectedNoSuspicious)
{
	// case 6b
	aggregator::UseList l;
	for ( int i = 0; i < 6; ++ i)
		l.push_back("00000");
	l[2] = "99319";

	aggregator::BaseUseInfo result = aggregate(l);

	EXPECT_EQ("99019", result);
}

TEST(FlagInheritanceTest, OneAutomaticallyCorrectedNoSuspicious)
{
	// case 6b
	aggregator::UseList l;
	for ( int i = 0; i < 6; ++ i)
		l.push_back("00000");
	l[2] = "99337";

	aggregator::BaseUseInfo result = aggregate(l);

	EXPECT_EQ("99137", result);
}

TEST(FlagInheritanceTest, OneManuallyInterpolatedSomeSuspicious)
{
	// case 6c
	aggregator::UseList l;
	for ( int i = 0; i < 6; ++ i)
		l.push_back("00000");
	l[2] = "98929";
	l[4] = "00109";

	aggregator::BaseUseInfo result = aggregate(l);

	EXPECT_EQ("99109", result);
}

TEST(FlagInheritanceTest, OneAutomaticallyInterpolatedSomeSuspicious)
{
	// case 6c
	aggregator::UseList l;
	for ( int i = 0; i < 6; ++ i)
		l.push_back("00000");
	l[2] = "98949";
	l[4] = "00109";

	aggregator::BaseUseInfo result = aggregate(l);

	EXPECT_EQ("99109", result);
}

TEST(FlagInheritanceTest, OneAutomaticallyCorrectedSomeSuspicious)
{
	// case 6c/2a
	aggregator::UseList l;
	for ( int i = 0; i < 6; ++ i)
		l.push_back("00000");
	l[1] = "00109";
	l[3] = "00209";
	l[4] = "00109";
	l[2] = "99337";

	aggregator::BaseUseInfo result = aggregate(l);

	EXPECT_EQ("99209", result);
}
