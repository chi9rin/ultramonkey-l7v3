AUTOMAKE_OPTIONS = foreign
L7VS_MODDIR      = @l7vs_moddir@
L7VS_INCLUDEDIR  = @l7vs_includedir@
MAX_BUFFER_SIZE  = @l7vs_max_buffer_size@
AM_CPPFLAGS = \
	-g -O2 -fno-strict-aliasing -Wall -Werror -fPIC -pthread \
	-I$(L7VS_INCLUDEDIR) \
	-DMAX_BUFFER_SIZE=$(MAX_BUFFER_SIZE)

lib_LTLIBRARIES = \
	libsched_rnd.la

libsched_rnd_la_SOURCES = \
	schedule_module_base.h \
	schedule_module_rnd.h \
	schedule_module_rnd.cpp

libsched_rnd_la_LIBADD = -lrt -lboost_thread-mt

install:
	cp ./.libs/libsched_rnd.so ./.libs/sched_rnd.so && \
	$(INSTALL) -m 755 -d $(L7VS_MODDIR)
	$(INSTALL) -m 755 -D \
		./.libs/sched_rnd.so \
		$(L7VS_MODDIR)
