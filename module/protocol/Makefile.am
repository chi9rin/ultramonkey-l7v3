AUTOMAKE_OPTIONS	= foreign
L7VS_MODDIR		= @l7vs_dir@
MAX_BUFFER_SIZE	= @l7vs_max_buffer_size@
AM_CPPFLAGS		= \
		-g -O2 -Wall -Werror -fPIC -pthread \
		-I../../include \
		-I../../logger \
		-I../../parameter \
		-DMAX_BUFFER_SIZE=$(MAX_BUFFER_SIZE)

lib_LTLIBRARIES	=	libprotomod_cinsert.la \
					libprotomod_sslid.la \
					libprotomod_sessionless.la

libprotomod_cinsert_la_SOURCES	=	protocol_module_base.h \
									http_protocol_module_base.h \
									http_protocol_module_base.cpp \
									protocol_module_cinsert.cpp
libprotomod_cinsert_la_LIBADD	=	-lrt -lboost_thread

libprotomod_sslid_la_SOURCES	=	protocol_module_base.h \
									ssl_protocol_module_base.h \
									ssl_protocol_module_base.cpp \
									protocol_module_sslid.cpp
libprotomod_sslid_la_LIBADD		=	-lrt -lboost_thread

libprotomod_sessionless_la_SOURCES	=	protocol_module_base.h \
									http_protocol_module_base.h \
									http_protocol_module_base.cpp \
									protocol_module_sessionless.cpp
libprotomod_sessionless_la_LIBADD	= -lrt -lboost_thread

install:
	$(INSTALL) -m 755 -d $(L7VS_MODDIR)
	$(INSTALL) -D 755 -D $(L7VS_MODDIR)
