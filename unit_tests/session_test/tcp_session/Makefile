#
#
#	Makefile for tcp_session_test Class unit test frameworks.
#
#
TARGET		= tcp_session_test_ut
CPP			= g++
CPPFLAGS	= -O0 -g -Werror -Wall -pthread -DMAX_BUFFER_SIZE=65535 -DDEBUG
INCLUDES	=	-I../../../parameter \
				-I./ \
				-I../../../src \
				-I../../../include \
				-I../logger_stb
LIBS		=	-lboost_system-gcc41-mt \
				-lboost_thread-gcc41-mt \
				-lboost_unit_test_framework-gcc41-mt

LDFLAGS		= -lrt -ldl

SRCS		=	\
			../logger_stb/logger.cpp \
			../../../src/data_buff_base.cpp \
			../../../src/tcp_data.cpp \
			../../../src/tcp_thread_message.cpp \
			../../../src/tcp_thread_message_que.cpp \
			../../../src/tcp_realserver_connect_socket_list.cpp \
			tcp_session_test.cpp

.SUFFIX:	.cpp .o
.cpp.o:
		$(CPP) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

OBJS		= $(SRCS:.cpp=.o)

all:	$(TARGET)

$(TARGET):	$(OBJS)
	$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(TARGET) $(OBJS)
