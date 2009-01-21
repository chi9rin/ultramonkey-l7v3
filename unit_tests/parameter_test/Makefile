
#
#	Makefile for module_control unit test frameworks.
#
#
TARGET		= parameter_impl_ut
TARGET2		= parameter_ut
TARGET3		= parameter_impl_thread_ut
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

LDFLAGS		= -lrt -ldl -pthread

SRCS		=	parameter_impl_test.cpp
SRCS2		=	parameter_test.cpp
SRCS3		=	parameter_impl_thread_test.cpp

OBJS		= $(SRCS:.cpp=.o)
OBJS2		= $(SRCS2:.cpp=.o)
OBJS3		= $(SRCS3:.cpp=.o)

all:	$(TARGET) $(TARGET2) $(TARGET3)

$(TARGET):	$(OBJS)
	$(CPP) $(INCLUDES) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

$(TARGET2):	$(OBJS2)
	$(CPP) $(INCLUDES) $(LDFLAGS) -o $@ $(OBJS2) $(LIBS)

$(TARGET3): $(OBJS3)
	$(CPP) $(INCLUDES) $(LDFLAGS) -o $@ $(OBJS3) $(LIBS)
clean:
	rm -f $(TARGET) $(OBJS) $(TARGET2) $(OBJS2) $(TARGET3) $(OBJS3) sample.ini

.cpp.o:
	$(CPP) $(CPPFLAGS) $(INCLUDES) -c $<

