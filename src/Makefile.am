AUTOMAKE_OPTIONS	= foreign
sbin_PROGRAMS		= l7vsd l7vsadm
PKGLIBDIR			= @l7vs_moddir@
CONFIG_SOCK_PATH	= @l7vsadm_sockdir@
MAX_BUFFER_SIZE		= @l7vs_max_buffer_size@
PARAMETER_FILE		= @l7vs_config@
L7VS_SBINDIR		= @sbindir@

l7vsd_CFLAGS		= \
			-O2 -g -Wall -Werror \
			-I../include \
			-I../logger \
			-I../parameter \
			-I../snmpagent \
			-DL7VS_MODULE_PATH="\"$(PKGLIBDIR)\"" \
			-DMAX_BUFFER_SIZE=$(MAX_BUFFER_SIZE) \
			-DPARAMETER_FILE=$(PARAMETER_FILE) \
			-DL7VS_CONFIG_SOCK_PATH="\"$(CONFIG_SOCK_PATH)\"" \
			-DLOGGER_PROCESS_VSD \
			-DVERSION=\"$(VERSION)\"

l7vsd_SOURCES	=	\
			../logger/logger.h \
			../parameter/parameter.h \
			../include/command_receiver.h \
			../include/command_session.h \
			../include/l7vs_command.h \
			../include/l7vs_replication.h \
			../include/l7vsd.h \
			../include/module_base.h \
			../include/module_control_base.h \
			../include/protocol_module_base.h \
			../include/protocol_module_control.h \
			../include/realserver.h \
			../include/realserver_element.h \
			../include/replication.h \
			../include/schedule_module_base.h \
			../include/schedule_module_control.h \
			../include/session_thread_control.h \
			../include/virtualservice.h \
			../include/virtualservice_element.h \
			l7vsd.cpp \
			protocol_module_control.cpp \
			snmpbridge.c

l7vsd_LDADD	= \
			../logger/libl7vsd_logger.a \
			../parameter/libl7vsd_parameter.a \
			-llog4cxx \
			-lrt \
			-lboost_thread-gcc41-mt \
			-lboost_system-gcc41-mt

l7vsadm_CFLAGS	= \
			-O2 -g -Wall -Werror \
			-I../include \
			-I../logger \
			-I../parameter \
			-I../snmpagent \
			-DL7VS_MODULE_PATH="\"$(PKGLIBDIR)\"" \
			-DMAX_BUFFER_SIZE=$(MAX_BUFFER_SIZE) \
			-DPARAMETER_FILE=$(PARAMETER_FILE) \
			-DL7VS_CONFIG_SOCK_PATH="\"$(CONFIG_SOCK_PATH)\"" \
			-DLOGGER_PROCESS_ADM \
			-DVERSION=\"$(VERSION)\"

l7vsadm_SOURCES	=	\
			../logger/logger_wrapper.h \
			../parameter/parameter_wrapper.h \
			protocol_module_control.cpp \
			l7vsadm.cpp

l7vsadm_LDADD	=	\
			../logger/libl7vsadm_logger.a \
			../parameter/libl7vsadm_parameter.a \
			-llog4cxx \
			-lrt \
			-lboost_thread-gcc41-mt \
			-lboost_system-gcc41-mt

install:
	$(INSTALL) -m 755 -d $(CONFIG_SOCK_PATH)
	$(INSTALL) -m 755 -D \
		./l7vsd \
		./l7vsadm  \
		$(L7VS_SBINDIR)
