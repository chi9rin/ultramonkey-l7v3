AUTOMAKE_OPTIONS	= foreign

L7VS_CONF_DIR	= @l7vs_config@
L7DIRECTORD_CONFDIR	= /etc/ha.d/conf
L7VS_SSL_CONFDIR	= /etc/l7vs/sslproxy

install:
	$(INSTALL) -b -m 644 -D ./conf/l7vs.cf $(L7VS_CONF_DIR)
	$(INSTALL) -m 644 -D ./conf/l7directord.cf.sample $(L7DIRECTORD_CONFDIR)/l7directord.cf.sample
	$(INSTALL) -b -m 644 -D ./conf/sslproxy.target.cf $(L7VS_SSL_CONFDIR)/sslproxy.target.cf
	$(INSTALL) -b -m 644 -D ./sslfiles/root.pem $(L7VS_SSL_CONFDIR)/root.pem
	$(INSTALL) -b -m 644 -D ./sslfiles/server.pem $(L7VS_SSL_CONFDIR)/server.pem
	$(INSTALL) -b -m 644 -D ./sslfiles/dh512.pem $(L7VS_SSL_CONFDIR)/dh512.pem
	$(INSTALL) -b -m 644 -D ./sslfiles/passwd.txt $(L7VS_SSL_CONFDIR)/passwd.txt
