bin_PROGRAMS = kvAgregated

kvAgregated_SOURCES = \
	configuration/AgregatorConfiguration.h \
	configuration/AgregatorConfiguration.cpp \
	proxy/IncomingHandler.h \
	proxy/IncomingHandler.cc \
	proxy/KvalobsProxy.h \
	proxy/KvalobsProxy.cc \
	proxy/KvDataReceiver.h \
	proxy/KvDataReceiver.cc \
	proxy/KvDataSaver.h \
	proxy/KvDataSaver.cc \
	proxy/Callback.h \
	proxy/CallbackCollection.h \
	proxy/CallbackCollection.cpp \
	proxy/ProxyDatabaseConnection.h \
	proxy/ProxyDatabaseConnection.cpp \
	AgregatorRunner.h \
	AgregatorRunner.cpp \
	AbstractAgregator.h \
	AbstractAgregator.cc \
	AgregatorHandler.h \
	AgregatorHandler.cc \
	KvDataFunctors.h \
	KvDataFunctors.cc \
	minmax.h \
	minmax.cc \
	ra2rr_12.h \
	ra2rr_12.cc \
	rr.h rr.cc \
	GenerateZero.h \
	GenerateZero.cc \
	BackProduction.h \
	BackProduction.cc \
	paramID.h \
	times.h \
	main.cc

AM_CPPFLAGS = \
	-DVERSION=\"$(VERSION)\" \
	$(kvcpp_CFLAGS) \
	$(BOOST_CPPFLAGS)


kvAgregated_LDADD = \
	$(kvcpp_LIBS) \
	$(BOOST_PROGRAM_OPTIONS_LIB) 


kvAgregated_CPPFLAGS = -D_REENTRANT $(AM_CPPFLAGS)


kvAgregated_LDFLAGS = -rpath $(pkglibdir) -export-dynamic



TESTS = kvAgregatedTest	

check_PROGRAMS = kvAgregatedTest
#EXTRA_PROGRAMS = kvAgregatedTest

#installcheck-local: kvAgregatedTest
#	./$<

kvAgregatedTest_SOURCES = \
	test/test.cc \
	$(kvAgregated_SOURCES:main.cc=) \
	test/AbstractAgregatorTest.cc \
	test/AbstractAgregatorTest.h \
	test/MinMaxTest.cc \
	test/ra2rr_12ForwardTest.cc \
	test/ra2rr_12Test.cc \
	test/AgregatorConfigurationTest.cc \
	test/KvalobsProxyTest.cc
	

kvAgregatedTest_CPPFLAGS = \
	$(kvAgregated_CPPFLAGS) \
	$(gtest_CFLAGS) \
	$(gmock_CFLAGS)

kvAgregatedTest_LDADD = \
	$(kvAgregated_LDADD) \
	$(gtest_LIBS) \
	$(gmock_LIBS)

CLEANFILES = kvAgregatedTest
