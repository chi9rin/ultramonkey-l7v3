#
#
#	Makefile for module_control unit test frameworks.
#
#
TARGET		= vs_base_ut vs_tcp_ut vs_tcp_thread_ut vs_tcp_thread_ut2 vs_tcp_it vs_tcp_sorry_ut
#TARGET		= vs_tcp_ut
CPP			= g++
CPPFLAGS	= -O0 -g -Wall -Werror -pthread -D__SNMPBRIDGE_H__ -DMAX_BUFFER_SIZE=65535 -DDEBUG
# CPPFLAGS	= -O2 -Wall -Werror -pthread -DMAX_BUFFER_SIZE=65535 -DDEBUG

PARAMETERDIR    = ./parameter_stub

INCLUDES	=		\
				-I./parameter_stub \
				-I./ \
				-I../../include \
				-I../../src \
				-I/usr/include
LIBS		=		-lboost_system-gcc41-mt \
				-lboost_thread-gcc41-mt \
				-lboost_unit_test_framework-gcc41-mt \
				-lboost_serialization-gcc41-mt \
				-llog4cxx \
                                -lparameter

LDFLAGS		= -lrt -ldl -lssl -L$(PARAMETERDIR)

LOGGER_PROCESS = -DLOGGER_PROCESS_VSD



BASESRCS		=	\
				stub.cpp \
				../../src/logger_implement_access.cpp \
                                ../../src/logger_logrotate_utility.cpp \
                                ../../src/logger_access_manager.cpp \
                                ../../src/strict_time_based_rolling_policy.cpp \
                                ../../src/time_and_size_based_rolling_policy.cpp \
				../../src/virtualservice_base.cpp \
				virtualservice_tcp.cpp \
				virtualservice_udp.cpp \
				virtualservice.cpp \
				vs_base_test.cpp




TCPSRCS		=	\
				stub.cpp \
				../../src/logger_implement_access.cpp \
                                ../../src/logger_logrotate_utility.cpp \
                                ../../src/logger_access_manager.cpp \
                                ../../src/strict_time_based_rolling_policy.cpp \
                                ../../src/time_and_size_based_rolling_policy.cpp \
				../../src/virtualservice_base.cpp \
				../../src/virtualservice_tcp.cpp \
				virtualservice_udp.cpp \
				virtualservice.cpp \
				vs_tcp_test.cpp



TCPTSRCS		=	\
				stub.cpp \
				../../src/logger_implement_access.cpp \
                                ../../src/logger_logrotate_utility.cpp \
                                ../../src/logger_access_manager.cpp \
                                ../../src/strict_time_based_rolling_policy.cpp \
                                ../../src/time_and_size_based_rolling_policy.cpp \
				../../src/virtualservice_base.cpp \
				../../src/virtualservice_tcp.cpp \
				virtualservice_udp.cpp \
				virtualservice.cpp \
				vs_tcp_thread_test.cpp


TCPT2SRCS		=	\
				stub.cpp \
				../../src/logger_implement_access.cpp \
                                ../../src/logger_logrotate_utility.cpp \
                                ../../src/logger_access_manager.cpp \
                                ../../src/strict_time_based_rolling_policy.cpp \
                                ../../src/time_and_size_based_rolling_policy.cpp \
                               ../../src/virtualservice_base.cpp \
                               ../../src/virtualservice_tcp.cpp \
				virtualservice_udp.cpp \
				virtualservice.cpp \
				vs_tcp_thread_test2.cpp


ITSRCS		=	\
				stub.cpp \
				../../src/logger_implement_access.cpp \
                                ../../src/logger_logrotate_utility.cpp \
                                ../../src/logger_access_manager.cpp \
                                ../../src/strict_time_based_rolling_policy.cpp \
                                ../../src/time_and_size_based_rolling_policy.cpp \
                               ../../src/virtualservice_base.cpp \
                               ../../src/virtualservice_tcp.cpp \
				virtualservice_udp.cpp \
				virtualservice.cpp \
				vs_tcp_it_ut.cpp

SORRYSRCS		=	\
				stub_sorry.cpp \
				../../src/logger_implement_access.cpp \
				../../src/logger_logrotate_utility.cpp \
				../../src/logger_access_manager.cpp \
				../../src/strict_time_based_rolling_policy.cpp \
 				../../src/time_and_size_based_rolling_policy.cpp \
				virtualservice_udp.cpp \
				virtualservice.cpp \
				vs_tcp_sorry_test.cpp

.SUFFIX:	.cpp .o
.cpp.o:
		$(CPP) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

BASEOBJS	= $(BASESRCS:.cpp=.o)

TCPOBJS		= $(TCPSRCS:.cpp=.o)

TPCTOBJS	= $(TCPTSRCS:.cpp=.o)

TPCT2OBJS	= $(TCPT2SRCS:.cpp=.o)

ITOBJS		= $(ITSRCS:.cpp=.o)

SORRYOBJS		= $(SORRYSRCS:.cpp=.o)

all:	$(TARGET)

vs_base_ut:	$(BASEOBJS)
	$(CPP) $(INCLUDES) -o vs_base_ut $(BASEOBJS) $(LDFLAGS) $(LIBS)

vs_tcp_ut:	$(TCPOBJS)
	$(CPP) $(INCLUDES) -o vs_tcp_ut $(TCPOBJS) $(LDFLAGS) $(LIBS)

vs_tcp_thread_ut: $(TPCTOBJS)
	$(CPP) $(INCLUDES) -o vs_tcp_thread_ut $(TPCTOBJS) $(LDFLAGS) $(LIBS)

vs_tcp_thread_ut2: $(TPCT2OBJS)
	$(CPP) $(INCLUDES) -o vs_tcp_thread_ut2 $(TPCT2OBJS) $(LDFLAGS) $(LIBS)

vs_tcp_it: $(ITOBJS)
	$(CPP) $(INCLUDES) -o vs_tcp_it $(ITOBJS) $(LDFLAGS) $(LIBS)

vs_tcp_sorry_ut: $(SORRYOBJS)
	$(CPP) $(INCLUDES) -o vs_tcp_sorry_ut $(SORRYOBJS) $(LDFLAGS) $(LIBS)

# $(TARGET):	$(OBJS)
# 	$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(TARGET) $(BASEOBJS) $(TCPOBJS) $(TPCTOBJS) $(TPCT2OBJS) $(ITOBJS) $(SORRYOBJS)

