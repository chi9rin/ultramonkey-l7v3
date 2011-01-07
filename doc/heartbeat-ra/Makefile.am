AUTOMAKE_OPTIONS = foreign

UML7_VERSION = @ultramonkeyl7_version@
HB2_TEMPDIR  = /usr/share/doc/$(UML7_VERSION)/heartbeat-ra

install:
	$(INSTALL) -b -m 644 -D ./logd.cf $(HB2_TEMPDIR)/logd.cf
	$(INSTALL) -b -m 644 -D ./ha.cf $(HB2_TEMPDIR)/ha.cf
	$(INSTALL) -b -m 600 -D ./authkeys $(HB2_TEMPDIR)/authkeys
	$(INSTALL) -b -m 600 -D ./cib.xml-sample $(HB2_TEMPDIR)/cib.xml
	$(INSTALL) -b -m 755 -D ./L7vsd $(HB2_TEMPDIR)/L7vsd
	$(INSTALL) -b -m 755 -D ./L7directord $(HB2_TEMPDIR)/L7directord
	$(INSTALL) -b -m 755 -D ./VIPcheck $(HB2_TEMPDIR)/VIPcheck
