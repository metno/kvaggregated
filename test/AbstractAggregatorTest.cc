#include "AbstractAggregatorTest.h"
#include <kvalobs/kvDataOperations.h>

using aggregator::StandardAggregator;

namespace
{
boost::posix_time::ptime pt(const std::string & s)
{
	return boost::posix_time::time_from_string(s);
}
boost::gregorian::date dt(const std::string & s)
{
	return boost::gregorian::date_from_iso_string(s);
}
}

TEST_P(AbstractAggregatorTest, testGetTimeSpanAtGenerationPoint)
{
	const std::set<boost::posix_time::time_duration> & generateWhen = GetParam()->generateWhen();
	const boost::posix_time::time_duration toTest(* generateWhen.begin());
	const kvalobs::kvDataFactory dataFactory( 42, pt("2007-06-06 06:00:00"), 302 );
	boost::posix_time::ptime time(dt("2007-06-06"),toTest);
	const kvalobs::kvData d = dataFactory.getData( 15, 1, time );
	
	const StandardAggregator::TimeSpan timeSpan = GetParam()->getTimeSpan(d);

	ASSERT_EQ(time, timeSpan.second);
	
	time -= boost::posix_time::hours(GetParam()->interestingHours());
	ASSERT_EQ(time, timeSpan.first);
}


TEST_P(AbstractAggregatorTest, testGetTimeSpan)
{
	const std::set<boost::posix_time::time_duration> & generateWhen = GetParam()->generateWhen();
	const boost::posix_time::time_duration toTest(* generateWhen.begin());
	const kvalobs::kvDataFactory dataFactory( 42, pt("2007-06-06 06:00:00"), 302 );
	boost::posix_time::ptime time(dt("2007-06-06"),toTest);
	
	boost::posix_time::ptime triggerTime = time - boost::posix_time::hours(1);
	const kvalobs::kvData d = dataFactory.getData( 15, 1, triggerTime );
	
	const StandardAggregator::TimeSpan timeSpan = GetParam()->getTimeSpan(d);
	
	ASSERT_EQ(time, timeSpan.second);
	
	time -= boost::posix_time::hours(GetParam()->interestingHours());
	ASSERT_EQ(time, timeSpan.first);
}

TEST(AbstractAggregatorFaggTest, test)
{
	kvalobs::kvDataFactory f(100, pt("2010-07-16 06:00:00"), 302);
	kvalobs::kvData d = f.getData(1, 100);

	aggregator::AbstractAggregator::kvDataList dl;

	d.controlinfo(kvalobs::kvControlInfo("0600604000000000"));
	d.useinfo(kvalobs::kvUseInfo("7033700000000002"));
	dl.push_back(d);

	EXPECT_EQ(6, aggregator::internal::calculateAggregateFlag_(dl));
}
