kvAgregated_SOURCES = \
	src/WorkLoop.h \
	src/WorkLoop.cpp \
	src/AgregatorRunner.h \
	src/AgregatorRunner.cpp \
	src/AbstractAgregator.h \
	src/AbstractAgregator.cc \
	src/AgregatorHandler.h \
	src/AgregatorHandler.cc \
	src/KvDataFunctors.h \
	src/KvDataFunctors.cc \
	src/minmax.h \
	src/minmax.cc \
	src/ra2rr_12.h \
	src/ra2rr_12.cc \
	src/rr.h \
	src/rr.cc \
	src/GenerateZero.h \
	src/GenerateZero.cc \
	src/BackProduction.h \
	src/BackProduction.cc \
	src/paramID.h \
	src/times.h \
	src/main.cc

AM_CPPFLAGS = \
	-DVERSION=\"$(VERSION)\" \
	$(kvcpp_CFLAGS) \
	$(BOOST_CPPFLAGS)


kvAgregated_LDADD = \
	$(kvcpp_LIBS) \
	$(BOOST_PROGRAM_OPTIONS_LIB) 


kvAgregated_CPPFLAGS = -D_REENTRANT $(AM_CPPFLAGS)


kvAgregated_LDFLAGS = -rpath $(pkglibdir) -export-dynamic

include src/configuration/kvAgregated.mk
include src/proxy/kvAgregated.mk
