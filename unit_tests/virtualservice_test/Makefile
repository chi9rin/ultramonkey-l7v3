#
#
#	Makefile for module_control unit test frameworks.
#
#
TARGET		= vs_base_ut vs_tcp_ut vs_tcp_thread_ut
CPP			= g++
CPPFLAGS	= -O0 -g -Wall -Werror -pthread -DMAX_BUFFER_SIZE=65535 -DDEBUG
INCLUDES	=	-I../../logger \
				-I../../parameter \
				-I./ \
				-I../../include \
				-I../../src \
				-I../../snmpagent
LIBS		=	-lboost_system-gcc41-mt \
				-lboost_thread-gcc41-mt \
				-lboost_unit_test_framework-gcc41-mt \
				-lboost_serialization-gcc41-mt

LDFLAGS		= -lrt -ldl

BASESRCS		=	\
				stub.cpp \
				../../src/virtualservice.cpp \
				vs_base_test.cpp

TCPSRCS		=	\
				stub.cpp \
				../../src/virtualservice.cpp \
				vs_tcp_test.cpp

TCPTSRCS		=	\
				stub.cpp \
				../../src/virtualservice.cpp \
				vs_tcp_thread_test.cpp

# UDPSRCS		=	\
# 				stub.cpp \
# 				../../src/virtualservice.cpp \
# 				vs_udp_test.cpp

.SUFFIX:	.cpp .o
.cpp.o:
		$(CPP) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

BASEOBJS	= $(BASESRCS:.cpp=.o)

TCPOBJS		= $(TCPSRCS:.cpp=.o)

TPCTOBJS	= $(TCPTSRCS:.cpp=.o)

# UDPOBJS		= $(UDPSRCS:.cpp=.o)

all:	$(TARGET)

vs_base_ut:	$(BASEOBJS)
	$(CPP) $(INCLUDES) -o vs_base_ut $(BASEOBJS) $(LDFLAGS) $(LIBS)

vs_tcp_ut:	$(TCPOBJS)
	$(CPP) $(INCLUDES) -o vs_tcp_ut $(TCPOBJS) $(LDFLAGS) $(LIBS)

vs_tcp_thread_ut: $(TPCTOBJS)
	$(CPP) $(INCLUDES) -o vs_tcp_thread_ut $(TPCTOBJS) $(LDFLAGS) $(LIBS)


# $(TARGET):	$(OBJS)
# 	$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(TARGET) $(BASEOBJS) $(TCPOBJS)
