AUTOMAKE_OPTIONS	= foreign
sbin_PROGRAMS		= l7vsd l7vsadm
PKGLIBDIR			= @l7vs_moddir@
CONFIG_SOCK_PATH	= @l7vsadm_sockdir@
MAX_BUFFER_SIZE		= @l7vs_max_buffer_size@
PARAMETER_FILE		= @l7vs_config@
L7VS_SBINDIR		= @sbindir@

l7vsd_CPPFLAGS		= \
			-O2 -g -Wall -Werror -pthread \
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
			../include/data_buff_base.h \
			../include/session_result_message.h \
			../include/tcp_data.h \
			../include/tcp_socket.h \
			../include/tcp_realserver_connetc_socket_list.h \
			../include/tcp_thread_message.h \
			../include/tcp_thread_message_que.h \
			../include/tcp_session.h \
			../include/udp_data.h \
			../include/udp_request.h \
			../include/udp_request_thread_control.h \
			../include/udp_session.h \
			../include/snmpbridge.h \
			l7vsd.cpp \
			virtualservice_base.cpp \
			virtualservice_tcp.cpp \
			virtualservice_udp.cpp \
			virtualservice.cpp \
			session_thread_control.cpp \
			protocol_module_control.cpp \
			schedule_module_control.cpp \
			command_receiver.cpp \
			command_session.cpp \
			replication.cpp \
			snmpbridge.cpp \
			data_buff_base.cpp \
			tcp_data.cpp \
			tcp_socket.cpp \
			tcp_realserver_connect_socket_list.cpp \
			tcp_thread_message_que.cpp \
			udp_data.cpp \
			udp_request_thread_control.cpp \
			udp_request.cpp \
			tcp_session.cpp \
			udp_session.cpp
l7vsd_LDADD	= \
			../logger/libl7vsd_logger.a \
			../parameter/libl7vsd_parameter.a \
			-llog4cxx \
			-lrt \
			-lboost_thread-gcc41-mt \
			-lboost_system-gcc41-mt \
			-lboost_serialization-gcc41-mt

l7vsadm_CPPFLAGS	= \
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
			../logger/logger.h \
			../parameter/parameter.h \
			protocol_module_control.cpp \
			schedule_module_control.cpp \
			l7vsadm.cpp

l7vsadm_LDADD	=	\
			../logger/libl7vsadm_logger.a \
			../parameter/libl7vsadm_parameter.a \
			-llog4cxx \
			-lrt \
			-lboost_thread-gcc41-mt \
			-lboost_system-gcc41-mt \
			-lboost_serialization-gcc41-mt \
			-lboost_regex-gcc41-mt

install:
	$(INSTALL) -m 755 -d $(CONFIG_SOCK_PATH)
	$(INSTALL) -m 755 -D \
		./l7vsd \
		./l7vsadm  \
		$(L7VS_SBINDIR)
