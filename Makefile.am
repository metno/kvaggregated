ACLOCAL_AMFLAGS = -I m4

bin_SCRIPTS = kvAgregateDbInit

pkgstatedir = $(localstatedir)/lib/kvagregated/database.sqlite
nodist_pkgstate_DATA = database.sqlite
MOSTLYCLEANFILES = database.sqlite

database.sqlite: 
	. $(srcdir)/kvAgregateDbInit.sh $@ 

EXTRA_DIST = kvAgregateDbInit.sh

kvAgregateDbInit: kvAgregateDbInit.sh
	cp $< $@
	chmod 744 $@

CLEANFILES = kvAgregateDbInit

bin_PROGRAMS = kvAgregated
include src/kvAgregated.mk
include doc/kvAgregated.mk
include test/kvAgregated.mk

include mk/debian.mk 