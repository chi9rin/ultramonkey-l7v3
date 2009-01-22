noinst_LIBRARIES= libl7vsd_parameter.a libl7vsadm_parameter.a libsnmpagent_parameter.a libsslproxy_parameter.a
AUTOMAKE_OPTIONS = foreign
		
libl7vsd_parameter_a_SOURCES		= \
				parameter_enum.h \
				parameter.h \
				parameter_impl.h \
				parameter.cpp \
				parameter_impl.cpp

libl7vsd_parameter_a_CPPFLAGS	= -O2 -g -I../include -I../logger \
				  -DLOGGER_PROCESS_VSD \
                                  -DVERSION=\"$(VERSION)\"

libl7vsadm_parameter_a_SOURCES		= \
				parameter_enum.h \
				parameter.h \
				parameter_impl.h \
				parameter.cpp \
				parameter_impl.cpp

libl7vsadm_parameter_a_CPPFLAGS	= -O2 -g -I../include -I../logger \
				  -DLOGGER_PROCESS_ADM \
				  -DVERSION=\"$(VERSION)\"

libsnmpagent_parameter_a_SOURCES		= \
				parameter_enum.h \
				parameter.h \
				parameter_impl.h \
				parameter.cpp \
				parameter_impl.cpp

libsnmpagent_parameter_a_CPPFLAGS	= -O2 -g -I../include -I../logger \
					  -DLOGGER_PROCESS_SNM \
                                          -DVERSION=\"$(VERSION)\"

libsslproxy_parameter_a_SOURCES			= \
				parameter_enum.h \
				parameter.h \
				parameter_impl.h \
				parameter.cpp \
				parameter_impl.cpp

libsslproxy_parameter_a_CPPFLAGS	= -O2 -g -I ../include -I../logger \
						-DLOGGER_PROCESS_SSL -DVERSION=\"$(VERSION)\"