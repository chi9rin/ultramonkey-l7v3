
#
#	Makefile for module_control unit test frameworks.
#
#
TARGET1		= parameter_ut
TARGET2		= parameter_impl_ut
TARGET3		= parameter_impl_thread_ut
CPP		= g++
CPPFLAGS	= -g -Wall -Werror -pthread -O0 -D PARAMETER_FILE='"./sample.ini"' -L../../src -DCASE1
INCLUDES	=  \
			-I../../src \
                        -I../../include
LIBS		= -L./logger_stub	\
				-lboost_system-gcc41-mt \
				-lboost_thread-gcc41-mt \
				-lboost_unit_test_framework-gcc41-mt \
				-llog4cxx \
				-llogger

LDFLAGS		= -lrt -ldl -pthread

SRCS1		=	parameter_test.cpp
SRCS2		=	parameter_impl_test.cpp
SRCS3		=	parameter_impl_thread_test.cpp

OBJS1		= $(SRCS1:.cpp=.o)
OBJS2		= $(SRCS2:.cpp=.o)
OBJS3		= $(SRCS3:.cpp=.o)

all:	$(TARGET1) $(TARGET2) $(TARGET3)

$(TARGET1):	$(OBJS1) 
	$(CPP) $(CPPFLAGS) $(INCLUDES) $(LDFLAGS) -o $@ $(OBJS1) $(LIBS)

$(TARGET2):	$(OBJS2)
	$(CPP) $(CPPFLAGS) $(INCLUDES) $(LDFLAGS) -o $@ $(OBJS2) $(LIBS)

$(TARGET3): $(OBJS3)
	$(CPP) $(CPPFLAGS) $(INCLUDES) $(LDFLAGS) -o $@ $(OBJS3) $(LIBS)
clean:
	rm -f $(TARGET1) $(OBJS1) $(TARGET2) $(OBJS2) $(TARGET3) $(OBJS3) sample.ini

.cpp.o:
	$(CPP) $(CPPFLAGS) $(INCLUDES) -c $<

