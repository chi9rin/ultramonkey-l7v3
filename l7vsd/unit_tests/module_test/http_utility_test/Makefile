#
#
#	Makefile for http_utility_base unit test frameworks.
#
#
TARGET		= http_utility_ut
CPP			= g++
CPPFLAGS	= -O0 -g -Werror -Wall -pthread -DMAX_BUFFER_SIZE=65535 -DDEBUG
INCLUDES	=	-I../../../logger \
				-I../../../parameter \
				-I../../../include \
				-I../../../module/protocol \
				-I../../../src
LIBS		=	-lboost_system-mt \
				-lboost_thread-mt \
				-lboost_regex-mt \
				-lboost_unit_test_framework-mt

LDFLAGS		= -lrt -ldl

SRCS		=	\
			../../../module/protocol/http_utility.cpp \
			http_utility_test.cpp


.SUFFIX:	.cpp .o
.cpp.o:
		$(CPP) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

OBJS		= $(SRCS:.cpp=.o)

all:	$(TARGET)

$(TARGET):	$(OBJS)
	$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(TARGET) $(OBJS)
