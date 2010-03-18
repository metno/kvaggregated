man_MANS = kvAgregated.1 kvAgregateDbInit.1

kvAgregated.1: doc/kvAgregated.man.xml
	$(XMLTO) man $<

kvAgregateDbInit.1: doc/kvAgregateDbInit.man.xml
	$(XMLTO) man $<

EXTRA_DIST += doc/kvAgregated.man.xml doc/kvAgregateDbInit.man.xml

CLEANFILES += $(man_MANS)
