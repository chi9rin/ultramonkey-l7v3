noinst_LIBRARIES= libl7vsd_parameter.a libl7vsadm_parameter.a libsnmpagent_parameter.a libsslproxy_parameter.a
AUTOMAKE_OPTIONS 	= foreign
PARAMETER_FILE		= @l7vs_config@

libl7vsd_parameter_a_SOURCES		= \
				parameter_enum.h \
				parameter.h \
				parameter_impl.h \
				parameter.cpp \
				parameter_impl.cpp

libl7vsd_parameter_a_CPPFLAGS	= \
				-O2 -g -Wall -Werror \
				-I../include \
				-I../logger \
				-DPARAMETER_FILE="\"$(PARAMETER_FILE)\"" \
				-DVERSION=\"$(VERSION)\" \
				-DLOGGER_PROCESS_VSD

libl7vsadm_parameter_a_SOURCES		= \
				parameter_enum.h \
				parameter.h \
				parameter_impl.h \
				parameter.cpp \
				parameter_impl.cpp

libl7vsadm_parameter_a_CPPFLAGS	= \
				-O2 -g -Wall -Werror \
				-I../include \
				-I../logger \
				-DPARAMETER_FILE="\"$(PARAMETER_FILE)\"" \
				-DVERSION=\"$(VERSION)\" \
				-DLOGGER_PROCESS_ADM 

libsnmpagent_parameter_a_SOURCES		= \
				parameter_enum.h \
				parameter.h \
				parameter_impl.h \
				parameter.cpp \
				parameter_impl.cpp

libsnmpagent_parameter_a_CPPFLAGS	= \
				-O2 -g -Wall -Werror \
				-I../include \
				-I../logger \
				-DPARAMETER_FILE="\"$(PARAMETER_FILE)\"" \
				-DVERSION=\"$(VERSION)\" \
				-DLOGGER_PROCESS_SNM

libsslproxy_parameter_a_SOURCES			= \
				parameter_enum.h \
				parameter.h \
				parameter_impl.h \
				parameter.cpp \
				parameter_impl.cpp

libsslproxy_parameter_a_CPPFLAGS	= \
				-O2 -g -Wall -Werror \
				-I../include \
				-I../logger \
				-DPARAMETER_FILE="\"$(PARAMETER_FILE)\"" \
				-DVERSION=\"$(VERSION)\" \
				-DLOGGER_PROCESS_SSL
