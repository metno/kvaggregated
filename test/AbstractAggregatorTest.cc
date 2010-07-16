#include "AbstractAggregatorTest.h"
#include <kvalobs/kvDataOperations.h>

using aggregator::StandardAggregator;

TEST_P(AbstractAggregatorTest, testGetTimeSpanAtGenerationPoint)
{
	const std::set<miutil::miClock> & generateWhen = GetParam()->generateWhen();
	const miutil::miClock toTest(* generateWhen.begin());
	const kvalobs::kvDataFactory dataFactory( 42, "2007-06-06 06:00:00", 302 );
	miutil::miTime time(miutil::miDate("2007-06-06"),toTest);
	const kvalobs::kvData d = dataFactory.getData( 15, 1, time );
	
	const StandardAggregator::TimeSpan timeSpan = GetParam()->getTimeSpan(d);

	ASSERT_EQ(time, timeSpan.second);
	
	time.addHour(- GetParam()->interestingHours());
	ASSERT_EQ(time, timeSpan.first);
}


TEST_P(AbstractAggregatorTest, testGetTimeSpan)
{
	const std::set<miutil::miClock> & generateWhen = GetParam()->generateWhen();
	const miutil::miClock toTest(* generateWhen.begin());
	const kvalobs::kvDataFactory dataFactory( 42, "2007-06-06 06:00:00", 302 );
	miutil::miTime time(miutil::miDate("2007-06-06"),toTest);
	
	miutil::miTime triggerTime = time;
	triggerTime.addHour(-1);
	const kvalobs::kvData d = dataFactory.getData( 15, 1, triggerTime );
	
	const StandardAggregator::TimeSpan timeSpan = GetParam()->getTimeSpan(d);
	
	ASSERT_EQ(time, timeSpan.second);
	
	time.addHour(- GetParam()->interestingHours());
	ASSERT_EQ(time, timeSpan.first);
}

TEST(AbstractAggregatorFaggTest, test)
{
	kvalobs::kvDataFactory f(100, "2010-07-16 06:00:00", 302);
	kvalobs::kvData d = f.getData(1, 100);

	aggregator::AbstractAggregator::kvDataList dl;

	d.controlinfo(kvalobs::kvControlInfo("0600604000000000"));
	d.useinfo(kvalobs::kvUseInfo("7033700000000002"));
	dl.push_back(d);

	EXPECT_EQ(6, aggregator::internal::calculateAggregateFlag_(dl));
}
