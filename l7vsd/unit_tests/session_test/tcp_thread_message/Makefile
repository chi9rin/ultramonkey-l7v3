#
#
#	Makefile for tcp_thread_message_test Class unit test frameworks.
#
#
TARGET		= tcp_thread_message_test_ut
CPP			= g++
CPPFLAGS	= -O0 -g -Werror -Wall -pthread -DMAX_BUFFER_SIZE=65535 -DDEBUG
INCLUDES	=	-I../logger_stb \
				-I../../../parameter \
				-I./ \
				-I../../../snmpagent \
				-I../../../src \
				-I../../../include
LIBS		=	-lboost_system-mt \
				-lboost_thread-mt \
				-lboost_unit_test_framework-mt

LDFLAGS		= -lrt -ldl

SRCS		=	\
			../logger_stb/logger.cpp \
			tcp_thread_message_test.cpp

.SUFFIX:	.cpp .o
.cpp.o:
		$(CPP) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

OBJS		= $(SRCS:.cpp=.o)

all:	$(TARGET)

$(TARGET):	$(OBJS)
	$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(TARGET) $(OBJS)
