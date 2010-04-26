AUTOMAKE_OPTIONS = foreign

noinst_LIBRARIES = \
	libl7vsd_parameter.a \
	libl7vsadm_parameter.a \
	libsnmpagent_parameter.a \
	libl7vsd_logger.a \
	libl7vsadm_logger.a \
	libsnmpagent_logger.a 

PARAMETER_FILE = @l7vs_config@

libl7vsd_parameter_a_SOURCES = \
	../include/parameter_enum.h \
	../include/parameter.h \
	../include/parameter_impl.h \
	parameter.cpp \
	parameter_impl.cpp

libl7vsd_parameter_a_CPPFLAGS = \
	-O2 -fno-strict-aliasing -g -Wall -Werror \
	-I../include \
	-DPARAMETER_FILE="\"$(PARAMETER_FILE)\"" \
	-DVERSION=\"$(VERSION)\" \
	-DLOGGER_PROCESS_VSD

libl7vsadm_parameter_a_SOURCES = \
	../include/parameter_enum.h \
	../include/parameter.h \
	../include/parameter_impl.h \
	parameter.cpp \
	parameter_impl.cpp

libl7vsadm_parameter_a_CPPFLAGS = \
	-O2 -fno-strict-aliasing -g -Wall -Werror \
	-I../include \
	-DPARAMETER_FILE="\"$(PARAMETER_FILE)\"" \
	-DVERSION=\"$(VERSION)\" \
	-DLOGGER_PROCESS_ADM 

libsnmpagent_parameter_a_SOURCES = \
	../include/parameter_enum.h \
	../include/parameter.h \
	../include/parameter_impl.h \
	parameter.cpp \
	parameter_impl.cpp

libsnmpagent_parameter_a_CPPFLAGS = \
	-O2 -fno-strict-aliasing -g -Wall -Werror \
	-I../include \
	-DPARAMETER_FILE="\"$(PARAMETER_FILE)\"" \
	-DVERSION=\"$(VERSION)\" \
	-DLOGGER_PROCESS_SNM

libl7vsd_logger_a_SOURCES = \
	../include/logger_enum.h \
	../include/logger_rotation_enum.h \
	../include/logger.h \
	../include/logger_impl.h \
	../include/appender_property.h \
	../include/logger_implement_access.h \
	../include/logger_access_manager.h \
	../include/logger_logrotate_utility.h \
	logger.cpp \
	logger_impl.cpp \
	logger_implement_access.cpp \
	logger_access_manager.cpp \
	logger_logrotate_utility.cpp \
	../include/strict_time_based_rolling_policy.h \
	strict_time_based_rolling_policy.cpp \
	../include/time_and_size_based_rolling_policy.h \
	time_and_size_based_rolling_policy.cpp

libl7vsd_logger_a_CPPFLAGS = \
	-O2 -fno-strict-aliasing -g -Wall -Werror -pthread -I../include \
	-DLOGGER_PROCESS_VSD \
	-DVERSION=\"$(VERSION)\"

libl7vsadm_logger_a_SOURCES = \
	../include/logger_enum.h \
	../include/logger_rotation_enum.h \
	../include/logger.h \
	../include/logger_impl.h \
	../include/appender_property.h \
	../include/logger_implement_access.h \
	../include/logger_access_manager.h \
	../include/logger_logrotate_utility.h \
	logger.cpp \
	logger_impl.cpp \
	logger_implement_access.cpp \
	logger_access_manager.cpp \
	logger_logrotate_utility.cpp \
	../include/strict_time_based_rolling_policy.h \
	strict_time_based_rolling_policy.cpp \
	../include/time_and_size_based_rolling_policy.h \
	time_and_size_based_rolling_policy.cpp

libl7vsadm_logger_a_CPPFLAGS = \
	-O2 -fno-strict-aliasing -g -Wall -Werror -pthread -I../include \
	-DLOGGER_PROCESS_ADM \
	-DVERSION=\"$(VERSION)\"

libsnmpagent_logger_a_SOURCES = \
	../include/logger_enum.h \
	../include/logger_rotation_enum.h \
	../include/logger.h \
	../include/appender_property.h \
	../include/logger_implement_access.h \
	../include/logger_access_manager.h \
	../include/logger_logrotate_utility.h \
	logger_impl.h \
	logger.cpp \
	logger_impl.cpp \
	logger_implement_access.cpp \
	logger_access_manager.cpp \
	logger_logrotate_utility.cpp \
	../include/strict_time_based_rolling_policy.h \
	strict_time_based_rolling_policy.cpp \
	../include/time_and_size_based_rolling_policy.h \
	time_and_size_based_rolling_policy.cpp

libsnmpagent_logger_a_CPPFLAGS = \
	-O2 -fno-strict-aliasing -g -Wall -Werror -pthread -I../include \
	-DLOGGER_PROCESS_SNM \
	-DVERSION=\"$(VERSION)\"

sbin_PROGRAMS    = l7vsd l7vsadm
PKGLIBDIR        = @l7vs_moddir@
CONFIG_SOCK_PATH = @l7vsadm_sockdir@
MAX_BUFFER_SIZE  = @l7vs_max_buffer_size@
L7VS_SBINDIR     = @sbindir@

l7vsd_CPPFLAGS = \
	-O2 -fno-strict-aliasing -g -Wall -Werror -pthread \
	-I../include \
	-I../../snmpagent \
	-DL7VS_MODULE_PATH="\"$(PKGLIBDIR)\"" \
	-DMAX_BUFFER_SIZE=$(MAX_BUFFER_SIZE) \
	-DPARAMETER_FILE="\"$(PARAMETER_FILE)\"" \
	-DL7VS_CONFIG_SOCK_PATH="\"$(CONFIG_SOCK_PATH)\"" \
	-DLOGGER_PROCESS_VSD \
	-DVERSION=\"$(VERSION)\"

l7vsd_SOURCES = \
	../include/logger.h \
	../include/parameter.h \
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
	../include/tcp_ssl_socket.h \
	../include/tcp_socket_option.h \
	../include/tcp_realserver_connetc_socket_list.h \
	../include/tcp_thread_message.h \
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
	tcp_ssl_socket.cpp \
	tcp_realserver_connect_socket_list.cpp \
	udp_data.cpp \
	udp_request_thread_control.cpp \
	udp_request.cpp \
	tcp_session.cpp \
	udp_session.cpp

l7vsd_LDADD = \
	libl7vsd_logger.a \
	libl7vsd_parameter.a \
	-llog4cxx \
	-lrt \
	-lssl \
	-lboost_thread-mt \
	-lboost_system-mt \
	-lboost_serialization-mt

l7vsadm_CPPFLAGS = \
	-O2 -fno-strict-aliasing -g -Wall -Werror \
	-I../include \
	-I../../snmpagent \
	-DL7VS_MODULE_PATH="\"$(PKGLIBDIR)\"" \
	-DMAX_BUFFER_SIZE=$(MAX_BUFFER_SIZE) \
	-DPARAMETER_FILE="\"$(PARAMETER_FILE)\"" \
	-DL7VS_CONFIG_SOCK_PATH="\"$(CONFIG_SOCK_PATH)\"" \
	-DLOGGER_PROCESS_ADM \
	-DVERSION=\"$(VERSION)\"

l7vsadm_SOURCES = \
	../include/logger.h \
	../include/parameter.h \
	protocol_module_control.cpp \
	schedule_module_control.cpp \
	l7vsadm.cpp

l7vsadm_LDADD = \
	libl7vsadm_logger.a \
	libl7vsadm_parameter.a \
	-llog4cxx \
	-lrt \
	-lboost_thread-mt \
	-lboost_system-mt \
	-lboost_serialization-mt \
	-lboost_regex-mt

install:
	$(INSTALL) -m 755 -d $(CONFIG_SOCK_PATH)
	$(INSTALL) -m 755 -D \
		./l7vsd \
		./l7vsadm  \
		$(L7VS_SBINDIR)
