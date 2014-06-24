AUTOMAKE_OPTIONS = foreign

install:
if CHECK_SYSTEMD
	$(INSTALL) -m 644 -D ./l7vsd.service ${prefix}/lib/systemd/system/l7vsd.service
else
	$(INSTALL) -m 755 -D ./l7vsd ${sysconfdir}/rc.d/init.d/l7vsd
endif
