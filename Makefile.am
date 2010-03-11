ACLOCAL_AMFLAGS = -I m4

bin_SCRIPTS = kvAgregateDbInit.sh

pkgstatedir = $(localstatedir)/$(PACKAGE)/agregate
nodist_pkgstate_DATA = database.sqlite
MOSTLYCLEANFILES = database.sqlite

database.sqlite: 
	. $(srcdir)/kvAgregateDbInit.sh $@ 

EXTRA_DIST = kvAgregateDbInit.sh


bin_PROGRAMS = kvAgregated
include src/kvAgregated.mk
include test/kvAgregated.mk


include mk/debian.mk 