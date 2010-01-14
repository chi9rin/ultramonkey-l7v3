AUTOMAKE_OPTIONS	= foreign

L7VS_CONF_DIR	= @l7vs_config@
L7DIRECTORD_CONFDIR	= /etc/ha.d/conf
L7VS_SSL_CONFDIR	= /etc/l7vs/sslproxy

install:
	$(INSTALL) -b -m 644 -D ./l7vs.cf $(L7VS_CONF_DIR)
	$(INSTALL) -m 644 -D ./l7directord.cf.sample $(L7DIRECTORD_CONFDIR)/l7directord.cf.sample
	$(INSTALL) -b -m 644 -D ./sslproxy.target.cf $(L7VS_SSL_CONFDIR)/sslproxy.target.cf
