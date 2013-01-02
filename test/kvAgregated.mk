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


if HAVE_COMPILED_GTEST
# nothing
else
check_LIBRARIES = libgtest_local.a
nodist_libgtest_local_a_SOURCES = libgtest_local.c 

gtest-all.o: $(gtest_BASE)/src/gtest/src/gtest-all.cc
	$(CXX) -c -I$(gtest_BASE)/src/gtest $< -o $@
libgtest_local.a: gtest-all.o
	$(AR) crf $@ $<
	
kvAgregatedTest_LDADD += ./libgtest_local.a
endif
