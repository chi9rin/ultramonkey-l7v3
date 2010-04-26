AUTOMAKE_OPTIONS = foreign

SBINDIR = @sbindir@
RCDIR   = /etc/init.d

install:
	$(INSTALL) -m 755 -D ./l7directord $(SBINDIR)/l7directord
	$(INSTALL) -m 755 -D ./init.d/l7directord $(RCDIR)/l7directord

