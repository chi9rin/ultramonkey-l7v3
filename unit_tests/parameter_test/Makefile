
#
#	Makefile for module_control unit test frameworks.
#
#
TARGET		= parameter_impl_ut
TARGET2		= parameter_ut
CPP			= g++
CPPFLAGS	= -g -Wall -Werror -pthread -O0 -D PARAMETER_FILE='"./sample.ini"'
INCLUDES	=	-I../../logger \
				-I../../parameter \
				-I../../include \
				-I../../src
LIBS		=	-lboost_system-gcc41-mt \
				-lboost_thread-gcc41-mt \
				-lboost_unit_test_framework-gcc41-mt \
				-llog4cxx \
				../../logger/liblogger.a

LDFLAGS		= -lrt -ldl

SRCS		=	parameter_impl_test.cpp
SRCS2		=	parameter_test.cpp

OBJS		= $(SRCS:.cpp=.o)
OBJS2		= $(SRCS2:.cpp=.o)

all:	$(TARGET) $(TARGET2)

$(TARGET):	$(OBJS)
	$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS)

$(TARGET2):	$(OBJS2)
	$(CPP) $(INCLUDES) -o $@ $(OBJS2) $(LIBS)

clean:
	rm -f $(TARGET) $(OBJS) $(TARGET2) $(OBJS2) sample.ini

.cpp.o:
	$(CPP) $(CPPFLAGS) $(INCLUDES) -c $<

