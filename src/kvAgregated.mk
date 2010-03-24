kvAgregated_SOURCES = \
	src/WorkLoop.h \
	src/WorkLoop.cpp \
	src/AgregatorRunner.h \
	src/AgregatorRunner.cpp \
	src/AgregatorHandler.h \
	src/AgregatorHandler.cc \
	src/KvDataFunctors.h \
	src/KvDataFunctors.cc \
	src/GenerateZero.h \
	src/GenerateZero.cc \
	src/BackProduction.h \
	src/BackProduction.cc \
	src/paramID.h \
	src/times.h \
	src/main.cc

AM_CPPFLAGS = \
	-I$(top_srcdir)/src \
	-DVERSION=\"$(VERSION)\" \
	$(kvcpp_CFLAGS) \
	$(BOOST_CPPFLAGS)


kvAgregated_LDADD = \
	$(kvcpp_LIBS) \
	$(BOOST_PROGRAM_OPTIONS_LIB) \
	$(BOOST_FILESYSTEM_LIB)


kvAgregated_CPPFLAGS = -D_REENTRANT $(AM_CPPFLAGS)


kvAgregated_LDFLAGS = -rpath $(pkglibdir) -export-dynamic

include src/agregator/kvAgregated.mk
include src/configuration/kvAgregated.mk
include src/proxy/kvAgregated.mk
