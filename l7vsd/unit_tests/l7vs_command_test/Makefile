#
#
#	Makefile for l7vs_command unit test frameworks.
#
#
TARGET		= l7vs_command_ut
CPP			= g++
CPPFLAGS	= -g -Wall -Werror -pthread -O0
INCLUDES	=	-I../../logger \
				-I../../parameter \
				-I../../include \
				-I../../src
LIBS		=	-lboost_system-mt \
				-lboost_thread-mt \
				-lboost_unit_test_framework-mt

LDFLAGS		= -lrt -ldl

SRCS		=	l7vs_command_ut.cpp

OBJS		= $(SRCS:.cpp=.o)

all:	$(TARGET)

$(TARGET):	$(OBJS)
	$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(TARGET) $(OBJS)

.cpp.o:
	$(CPP) $(CFLAGS) $(INCLUDES) -c $<

