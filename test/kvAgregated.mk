TESTS = kvAgregatedTest	

check_PROGRAMS = kvAgregatedTest

kvAgregatedTest_SOURCES = \
	test/test.cc \
	$(kvAgregated_SOURCES:src/main.cc=) \
	test/AbstractAggregatorTest.cc \
	test/AbstractAggregatorTest.h \
	test/MinMaxTest.cc \
	test/ra2rr_12ForwardTest.cc \
	test/ra2rr_12Test.cc \
	test/AggregatorConfigurationTest.cc \
	test/KvalobsProxyTest.cc \
	test/AggregatorHandlerTest.cc \
	test/MeanValueAggregatorTest.cc \
	test/nn_24Test.cc \
	test/uu_24Test.cc \
	test/ta_24Test.cc \
	test/poTest.cc \
	test/rr_1Test.cc \
	test/ot_24Test.cc \
	test/checkDecisionTest.cc
	

kvAgregatedTest_CPPFLAGS = \
	-I$(top_srcdir)/src/ \
	$(kvAgregated_CPPFLAGS) \
	$(gtest_CFLAGS) \
	$(gmock_CFLAGS)

kvAgregatedTest_LDFLAGS = $(gtest_LDFLAGS)

kvAgregatedTest_LDADD = \
	$(kvAgregated_LDADD) \
	$(gtest_LIBS) \
	$(gmock_LIBS)


kvAgregatedTest_LDADD += $(gtest_LIBS) -lgtest_main

include mk/gtest.mk
