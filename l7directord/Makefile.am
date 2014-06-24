AUTOMAKE_OPTIONS = foreign

SBINDIR = @sbindir@

install:
	$(INSTALL) -m 755 -D ./l7directord $(SBINDIR)/l7directord
if CHECK_SYSTEMD
	$(INSTALL) -m 644 -D ./init.d/l7directord.service ${prefix}/lib/systemd/system/l7directord.service
else
	$(INSTALL) -m 755 -D ./init.d/l7directord ${sysconfdir}/rc.d/init.d/l7directord
endif
