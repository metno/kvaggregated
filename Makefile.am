ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src


bin_SCRIPTS = kvAgregateDbInit.sh

pkgstatedir = $(localstatedir)/$(PACKAGE)/agregate
nodist_pkgstate_DATA = database.sqlite
MOSTLYCLEANFILES = database.sqlite

database.sqlite: 
	. $(srcdir)/kvAgregateDbInit.sh $@ 

EXTRA_DIST = kvAgregateDbInit.sh
