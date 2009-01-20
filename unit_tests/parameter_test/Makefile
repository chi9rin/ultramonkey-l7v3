
#
#	Makefile for module_control unit test frameworks.
#
#
TARGET		= parameter_ut
CPP			= g++
CPPFLAGS	= -g -Wall -Werror -pthread -O0 -D PARAMETER_FILE='"./sample.ini"'
INCLUDES	=	-I../../logger \
				-I../../parameter \
				-I../../include \
				-I../../src
LIBS		=	-lboost_system-gcc41-mt \
				-lboost_thread-gcc41-mt \
				-lboost_unit_test_framework-gcc41-mt

LDFLAGS		= -lrt -ldl

SRCS		=	parameter_impl_test.cpp

OBJS		= $(SRCS:.cpp=.o)

all:	$(TARGET)

$(TARGET):	$(OBJS)
	$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(TARGET) $(OBJS)

.cpp.o:
	$(CPP) $(CPPFLAGS) $(INCLUDES) -c $<

