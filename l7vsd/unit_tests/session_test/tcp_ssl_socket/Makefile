#
#
#	Makefile for tcp_ssl_socket_test Class unit test frameworks.
#
#
TARGET		= tcp_ssl_socket_test_ut
CPP			= g++
CPPFLAGS	= -O0 -g -Werror -Wall -pthread -DMAX_BUFFER_SIZE=65535 -DDEBUG
INCLUDES	=		-I/usr/include \
				-I./ \
				-I../../../src \
				-I../../../include \
				-I../logger_stb
LIBS		=	-lboost_system-gcc41-mt \
				-lboost_thread-gcc41-mt \
				-lboost_unit_test_framework-gcc41-mt \
				-lssl


LDFLAGS		= -lrt -ldl

SRCS		=	\
			../logger_stb/logger.cpp \
			../../../src/tcp_ssl_socket.cpp \
			tcp_ssl_socket_test.cpp

.SUFFIX:	.cpp .o
.cpp.o:
		$(CPP) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

OBJS		= $(SRCS:.cpp=.o)

all:	$(TARGET)

$(TARGET):	$(OBJS)
	$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(TARGET) $(OBJS)
