AUTOMAKE_OPTIONS	= foreign

L7VS_SSL_CONFDIR	= /etc/l7vs/sslproxy

install:
	$(INSTALL) -b -m 644 -D ./root.pem $(L7VS_SSL_CONFDIR)/root.pem
	$(INSTALL) -b -m 644 -D ./server.pem $(L7VS_SSL_CONFDIR)/server.pem
	$(INSTALL) -b -m 644 -D ./dh512.pem $(L7VS_SSL_CONFDIR)/dh512.pem
	$(INSTALL) -b -m 644 -D ./passwd.txt $(L7VS_SSL_CONFDIR)/passwd.txt
