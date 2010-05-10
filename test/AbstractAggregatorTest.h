#ifndef ABSTRACTAGREGATORTEST_H_
#define ABSTRACTAGREGATORTEST_H_

#include <gtest/gtest.h>
#include <aggregator/StandardAggregator.h>
#include <boost/shared_ptr.hpp>


typedef boost::shared_ptr<aggregator::StandardAggregator> AggregatorPtr;

class AbstractAggregatorTest : public testing::TestWithParam<AggregatorPtr>
{};

#endif /*ABSTRACTAGREGATORTEST_H_*/
