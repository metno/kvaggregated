kvAgregated_SOURCES = \
	src/WorkLoop.h \
	src/WorkLoop.cpp \
	src/AggregatorRunner.h \
	src/AggregatorRunner.cpp \
	src/AggregatorHandler.h \
	src/AggregatorHandler.cc \
	src/KvDataFunctors.h \
	src/KvDataFunctors.cc \
	src/BackProduction.h \
	src/BackProduction.cc \
	src/paramID.h \
	src/times.h \
	src/metrics.h \
	src/metrics.cc \
	src/main.cc

AM_CPPFLAGS = \
	-I$(top_srcdir)/src \
	-DVERSION=\"$(VERSION)\" \
	$(kvcpp_CFLAGS) \
	$(BOOST_CPPFLAGS)


kvAgregated_LDADD = \
	$(kvcpp_LIBS) \
	-lkvalobs_kvalobs \
	-lkvalobs_milog \
	-lkvalobs_dnmithread \
	-lkvalobs_fileutil \
	-lkvalobs_kvdb \
	-lkvalobs_decodeutility \
	$(BOOST_THREAD_LIB) \
	$(BOOST_PROGRAM_OPTIONS_LIB) \
	$(BOOST_FILESYSTEM_LIB) \
	$(BOOST_SYSTEM_LIB) \
	$(BOOST_DATE_TIME_LIB)

	
kvAgregated_CPPFLAGS = -D_REENTRANT $(AM_CPPFLAGS)


kvAgregated_LDFLAGS = -rpath $(pkglibdir) -export-dynamic

include src/aggregator/kvAgregated.mk
include src/configuration/kvAgregated.mk
include src/proxy/kvAgregated.mk
include src/checkDecision/kvAgregated.mk
