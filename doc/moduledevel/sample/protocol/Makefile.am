AUTOMAKE_OPTIONS = foreign
L7VS_MODDIR      = @l7vs_moddir@
L7VS_INCLUDEDIR  = @l7vs_includedir@
MAX_BUFFER_SIZE  = @l7vs_max_buffer_size@

AM_CPPFLAGS      = \
	-g -O2 -fno-strict-aliasing -Wall -Werror -fPIC -pthread \
	-I$(L7VS_INCLUDEDIR) \
	-DMAX_BUFFER_SIZE=$(MAX_BUFFER_SIZE)

lib_LTLIBRARIES = \
        libprotomod_simple.la

libprotomod_simple_la_SOURCES = \
	protocol_module_base.h \
	http_protocol_module_base.h \
	http_protocol_module_base.cpp \
	protocol_module_simple.cpp

install:
	cp ./.libs/libprotomod_simple.so ./.libs/protomod_simple.so && \
	$(INSTALL) -m 755 -d $(L7VS_MODDIR)
	$(INSTALL) -m 755 -D \
		./.libs/protomod_simple.so $(L7VS_MODDIR)
