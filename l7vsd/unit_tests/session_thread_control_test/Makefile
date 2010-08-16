#
#
#	Makefile for module_control unit test frameworks.
#
#
TARGET		= stc_ut
CPP			= g++
CPPFLAGS	= -O0 -g -Wall -Werror -pthread -D__SNMPBRIDGE_H__ -DMAX_BUFFER_SIZE=65535
INCLUDES	=	-I logger_stb \
				-I./ \
				-I../../src \
				-I../../include \
				-I../../.. \
				-I/usr/include
LIBS		=	-lboost_system-mt \
				-lboost_thread-mt \
				-lboost_unit_test_framework-mt \
				-lboost_serialization-mt

LDFLAGS		= -lrt -ldl -lssl

SRCS		=	\
			stub.cpp \
			logger_stb/logger.cpp \
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
