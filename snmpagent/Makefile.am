bin_PROGRAMS= l7snmpagent
AUTOMAKE_OPTIONS = foreign
		
l7snmpagent_SOURCES	=	\
				../logger/logger_wrapper.h \
				../parameter/parameter_wrapper.h \
				UltraMonkeyL7.h \
				dispatch_message.h \
				interproccom.h \
				l7ag_socket.h \
				proc_mibcollectmessage.h \
				proc_settingrequest.h \
				proc_trapsendrequest.h \
				queue_sendtrap.h \
				store_mibdata.h \
				subagent.h \
				tag_threadstat.h \
				tag_trapid.h \
				watch_l7vsd.h \
				dispatch_message.cpp \
				l7ag_socket.cpp \
				interproccom.cpp \
				proc_mibcollectmessage.cpp \
				proc_settingrequest.cpp \
				proc_trapsendrequest.cpp \
				queue_sendtrap.cpp \
				store_mibdata.cpp \
				subagent.cpp \
				watch_l7vsd.cpp \
				l7snmpagent.cpp 

l7snmpagent_CPPFLAGS	=	-O2 -g -Wall -Werror -I../../include -I../logger -I../parameter -DLOGGER_PROCESS_SNM -DVERSION=\"$(VERSION)\"

l7snmpagent_LDADD	= \ 
				../logger/libsnmpagent_logger.a \
				../parameter/libparameter.a \
				-llog4cxx \
				-lrt

