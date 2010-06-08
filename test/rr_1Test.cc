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
#include <aggregator/rr_1.h>
#include <boost/assign/list_of.hpp>

class TestingRR1 : public aggregator::rr_1
{
public:
	TestingRR1() : aggregator::rr_1(false) {}

	// Making protected functions and typedefs public:
	using aggregator::rr_1::ValueList;
	using aggregator::rr_1::calculate;
};

TEST(tt_1Test, sumsCorrectly)
{
	TestingRR1 rr1;

	TestingRR1::ValueList values = boost::assign::list_of(1.1)(2.1)(3.1)(4.1);

	float sum = rr1.calculate(values, 0);

	EXPECT_FLOAT_EQ(10.4, sum);
}

TEST(tt_1Test, ignoreNegativeValues)
{
	TestingRR1 rr1;
	TestingRR1::ValueList values = boost::assign::list_of(0.1)(0.2)(-0.1)(0.3)(0.4)(0.5);

	float sum = rr1.calculate(values, 0);

	EXPECT_FLOAT_EQ(1.5, sum);
}
