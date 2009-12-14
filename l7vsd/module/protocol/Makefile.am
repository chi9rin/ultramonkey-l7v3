AUTOMAKE_OPTIONS	= foreign
L7VS_MODDIR		= @l7vs_moddir@
MAX_BUFFER_SIZE	= @l7vs_max_buffer_size@
AM_CPPFLAGS		= \
		-g -O0 -Wall -Werror -fPIC -pthread \
		-I../../include \
		-I../../logger \
		-I../../parameter \
		-DMAX_BUFFER_SIZE=$(MAX_BUFFER_SIZE)

lib_LTLIBRARIES	=	libprotomod_cinsert.la \
					libprotomod_sslid.la \
					libprotomod_sessionless.la \
					libprotomod_ip.la

libprotomod_cinsert_la_SOURCES	=	protocol_module_base.h \
									http_protocol_module_base.h \
									http_protocol_module_base.cpp \
									protocol_module_cinsert.cpp
libprotomod_cinsert_la_LIBADD	=	-lrt -lboost_thread-gcc41-mt

libprotomod_sslid_la_SOURCES	=	protocol_module_base.h \
									ssl_protocol_module_base.h \
									ssl_protocol_module_base.cpp \
									protocol_module_sslid.cpp \
									sslid_replication_data_processor.h \
									sslid_replication_data_processor.cpp \
									sslid_session_data_processor.h \
									sslid_session_data_processor.cpp
libprotomod_sslid_la_LIBADD		=	-lrt -lboost_thread-gcc41-mt

libprotomod_sessionless_la_SOURCES	=	protocol_module_base.h \
									http_protocol_module_base.h \
									http_protocol_module_base.cpp \
									protocol_module_sessionless.cpp
libprotomod_sessionless_la_LIBADD	= -lrt -lboost_thread-gcc41-mt

libprotomod_ip_la_SOURCES	=	protocol_module_base.h \
									ip_protocol_module_base.h \
									protocol_module_ip.cpp \
									ip_replication_data_processor.h \
									ip_replication_data_processor.cpp \
									ip_session_data_processor.h \
									ip_session_data_processor.cpp \
									http_utility.h \
									http_utility.cpp
libprotomod_ip_la_LIBADD		=	-lrt -lboost_thread-gcc41-mt



install:
	cp ./.libs/libprotomod_cinsert.so ./.libs/protomod_cinsert.so && \
	cp ./.libs/libprotomod_sslid.so ./.libs/protomod_sslid.so && \
	cp ./.libs/libprotomod_sessionless.so ./.libs/protomod_sessionless.so && \
	cp ./.libs/libprotomod_ip.so ./.libs/protomod_ip.so && \
	$(INSTALL) -m 755 -d $(L7VS_MODDIR)
	$(INSTALL) -m 755 -D \
		./.libs/protomod_cinsert.so \
		./.libs/protomod_sslid.so  \
		./.libs/protomod_ip.so  \
		./.libs/protomod_sessionless.so $(L7VS_MODDIR)
