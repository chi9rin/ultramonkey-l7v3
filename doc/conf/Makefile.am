AUTOMAKE_OPTIONS = foreign

L7VS_CONF_DIR       = @l7vs_config@
L7DIRECTORD_CONFDIR = ${sysconfdir}/ha.d/conf
L7VS_SSL_CONFDIR    = ${sysconfdir}/l7vs/sslproxy
L7DIRECTORD_LOGROTATEDIR = ${sysconfdir}/logrotate.d

install:
	$(INSTALL) -b -m 644 -D ./l7vs.cf $(L7VS_CONF_DIR)/l7vs.cf
	$(INSTALL) -m 644 -D ./l7directord.cf.sample $(L7DIRECTORD_CONFDIR)/l7directord.cf.sample
	$(INSTALL) -b -m 644 -D ./sslproxy.target.cf $(L7VS_SSL_CONFDIR)/sslproxy.target.cf
	$(INSTALL) -m 644 -D ./l7directord $(L7DIRECTORD_LOGROTATEDIR)/l7directord
