AUTOMAKE_OPTIONS	= foreign
L7VS_MODDIR		= @l7vs_dir@
MAX_BUFFER_SIZE	= @l7vs_max_buffer_size@
AM_CPPFLAGS		= \
		-g -O2 -Wall -Werror -fPIC -pthread \
		-I../../include \
		-I../../logger \
		-I../../parameter \
		-DMAX_BUFFER_SIZE=$(MAX_BUFFER_SIZE)

lib_LTLIBRARIES	=	libsched_lc.la \
					libsched_rr.la \
					libsched_wrr.la

libsched_lc_la_SOURCES	=	schedule_module_base.h \
							schedule_module_lc.h \
							schedule_module_lc.cpp
libsched_lc_la_LIBADD	=	-lrt -lboost_thread

libsched_rr_la_SOURCES	=	schedule_module_base.h \
							schedule_module_rr.h \
							schedule_module_rr.cpp
libsched_rr_la_LIBADD	=	-lrt -lboost_thread

libsched_wrr_la_SOURCES	=	schedule_module_base.h \
							schedule_module_wrr.h \
							schedule_module_wrr.cpp
libsched_wrr_la_LIBADD	=	-lrt -lboost_thread

install:
	$(INSTALL) -m 755 -d $(L7VS_MODDIR)
	$(INSTALL) -D 755 -D $(L7VS_MODDIR)
