#
#
#	Makefile for tcp_realserver_connect_socket_list_test Class unit test frameworks.
#
#
TARGET		= tcp_realserver_connect_socket_list_test_ut
CPP			= g++
CPPFLAGS	= -O0 -g -Werror -Wall -pthread -DMAX_BUFFER_SIZE=65535 -DDEBUG
INCLUDES	=	-I../../../parameter \
				-I./ \
				-I../../../src \
				-I../../../include \
				-I../logger_stb
LIBS		=	-lboost_system-mt \
				-lboost_thread-mt \
				-lboost_unit_test_framework-mt

LDFLAGS		= -lrt -ldl

SRCS		=	\
			../logger_stb/logger.cpp \
			tcp_realserver_connect_socket_list_test.cpp

.SUFFIX:	.cpp .o
.cpp.o:
		$(CPP) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

OBJS		= $(SRCS:.cpp=.o)

all:	$(TARGET)

$(TARGET):	$(OBJS)
	$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(TARGET) $(OBJS)
