#
#
#	Makefile for module_control unit test frameworks.
#
#
TARGET		= stc_ut
CPP			= g++
CPPFLAGS	= -O0 -g -Wall -Werror -pthread -DMAX_BUFFER_SIZE=65535
INCLUDES	=	\
				-I./ \
				-I../../logger \
				-I../../parameter \
				-I../../src \
				-I../../include \
				-I../../snmpagent
LIBS		=	-lboost_system-gcc41-mt \
				-lboost_thread-gcc41-mt \
				-lboost_unit_test_framework-gcc41-mt \
				-lboost_serialization-gcc41-mt

LDFLAGS		= -lrt -ldl

SRCS		=	\
			stub.cpp \
			../../src/virtualservice_base.cpp \
			../../src/virtualservice_tcp.cpp \
			../../src/virtualservice_udp.cpp \
			../../src/virtualservice.cpp \
			../../src/session_thread_control.cpp \
			stc_test.cpp

.SUFFIX:	.cpp .o
.cpp.o:
		$(CPP) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

OBJS		= $(SRCS:.cpp=.o)

all:	$(TARGET)

stc_ut: $(OBJS)
	$(CPP) $(INCLUDES) -o stc_ut $(OBJS) $(LDFLAGS) $(LIBS)

clean:
	rm -f $(TARGET) $(OBJS)
