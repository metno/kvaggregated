#ifndef ABSTRACTAGREGATORTEST_H_
#define ABSTRACTAGREGATORTEST_H_

#include <gtest/gtest.h>
#include <aggregator/AbstractAggregator.h>
#include <boost/shared_ptr.hpp>


typedef boost::shared_ptr<aggregator::AbstractAggregator> AggregatorPtr;

class AbstractAggregatorTest : public testing::TestWithParam<AggregatorPtr>
{};

#endif /*ABSTRACTAGREGATORTEST_H_*/
