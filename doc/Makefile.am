AUTOMAKE_OPTIONS	= foreign

L7VS_CONF_DIR	= @l7vs_config@
L7DIRECTORD_CONFDIR	= /etc/ha.d/conf

install:
	$(INSTALL) -b -m 644 -D ./conf/l7vs.cf $(L7VS_CONF_DIR)
	$(INSTALL) -m 644 -D ./conf/l7directord.cf.sample $(L7DIRECTORD_CONFDIR)/l7directord.cf.sample
