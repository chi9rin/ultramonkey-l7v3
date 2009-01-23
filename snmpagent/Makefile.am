bin_PROGRAMS= l7snmpagent
AUTOMAKE_OPTIONS = foreign
SNMPAGENT_OPTS = @snmpagent_libs@

l7snmpagent_SOURCES	=	\
				../logger/logger_wrapper.h \
				../parameter/parameter_wrapper.h \
				lockedqueue.h \
				QueueContainer.h \
				MessengerClient.h \
				dispatcher.h \
				store_mibdata.h \
				netsnmp_bridge.h \
				subagent.h \
				tag_threadstat.h \
				MessengerClient.cpp \
				dispatcher.cpp \
				store_mibdata.cpp \
				netsnmp_bridge.c \
				subagent.cpp \
				l7snmpagent.cpp 

l7snmpagent_CPPFLAGS	=	-O2 -Wall -Werror -g -pthread -I../include -I../logger -I../parameter -I/usr/include/ucd-snmp -DLOGGER_PROCESS_SNM


l7snmpagent_LDADD	=	\
				../logger/libsnmpagent_logger.a \
				../parameter/libsnmpagent_parameter.a \
				-llog4cxx \
				-lboost_thread-gcc41-mt \
				$(SNMPAGENT_OPTS) \
				-lrt

