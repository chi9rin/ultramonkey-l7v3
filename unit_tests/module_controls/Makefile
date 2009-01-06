#
#
#	Makefile for module_control unit test frameworks.
#
#
TARGET		= module_control_ut
CPP			= g++
CPPFLAGS	= -g -Wall -Werror -pthread -O0
INCLUDES	=	-I../../logger \
				-I../../parameter \
				-I../../include \
				-I../../src
LIBS		=	-lboost_system-gcc41-mt \
				-lboost_thread-gcc41-mt \
				-lboost_unit_test_framework-gcc41-mt

LDFLAGS		= -lrt -ldl

SRCS		=	protocol_module_control_test.cpp

OBJS		= $(SRCS:.cpp=.o)

all:	$(TARGET)

$(TARGET):	$(OBJS)
	$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(TARGET) $(OBJS)

.cpp.o:
	$(CPP) $(CFLAGS) $(INCLUDES) -c $<

