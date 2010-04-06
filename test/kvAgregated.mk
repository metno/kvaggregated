TESTS = kvAgregatedTest	

check_PROGRAMS = kvAgregatedTest

kvAgregatedTest_SOURCES = \
	test/test.cc \
	$(kvAgregated_SOURCES:src/main.cc=) \
	test/AbstractAgregatorTest.cc \
	test/AbstractAgregatorTest.h \
	test/MinMaxTest.cc \
	test/ra2rr_12ForwardTest.cc \
	test/ra2rr_12Test.cc \
	test/AgregatorConfigurationTest.cc \
	test/KvalobsProxyTest.cc \
	test/AgregatorHandlerTest.cc \
	test/MeanValueAgregatorTest.cc \
	test/nn_24Test.cc \
	test/FlagInheritanceTest.cc
	
	

kvAgregatedTest_CPPFLAGS = \
	-I$(top_srcdir)/src/ \
	$(kvAgregated_CPPFLAGS) \
	$(gtest_CFLAGS) \
	$(gmock_CFLAGS)

kvAgregatedTest_LDADD = \
	$(kvAgregated_LDADD) \
	$(gtest_LIBS) \
	$(gmock_LIBS)

CLEANFILES += kvAgregatedTest
