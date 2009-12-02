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
LIBS		=	-lboost_system-gcc41-mt \
				-lboost_thread-gcc41-mt \
				-lboost_regex-gcc41-mt \
				-lboost_unit_test_framework-gcc41-mt

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
