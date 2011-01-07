#
#
#	Makefile for command thread unit test frameworks.
#
#
TARGET		= command_thread
CPP			= g++
CPPFLAGS	= -Wall -O0 -g -Werror -pthread -D PARAMETER_FILE='"./l7vs.cf"'
INCLUDES        =       -I../../include
#INCLUDES	=	-I../../logger \
#				-I../../parameter \
#				-I../../include
LIBS		=	-lboost_system-mt \
				-lboost_thread-mt \
				-lboost_serialization-mt \
				-lboost_unit_test_framework-mt \
				-llog4cxx 

LDFLAGS		= -lrt -ldl

LOGGER		= libtest_logger.a
PARAMETER	= ligtest_parameter.a
AR			= ar
ARFLAGS		= cru

SRCS		=	\
			stub.cpp \
			command_thread_test.cpp

PARAMETER_SRCS	=	\
                        ../../src/parameter.cpp \
                        ../../src/parameter_impl.cpp
#			../../parameter/parameter.cpp \
#			../../parameter/parameter_impl.cpp

LOGGER_SRCS	=	\
                        ../../src/logger.cpp \
                        ../../src/logger_impl.cpp \
                        ../../src/strict_time_based_rolling_policy.cpp \
                        ../../src/time_and_size_based_rolling_policy.cpp
#			../../logger/logger.cpp \
#			../../logger/logger_impl.cpp \
#			../../logger/strict_time_based_rolling_policy.cpp \
#			../../logger/time_and_size_based_rolling_policy.cpp

.SUFFIXES:	.o .cpp
.cpp.o:
		$(CPP) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

OBJS		= $(SRCS:.cpp=.o)
PARAMETER_OBJS	= $(PARAMETER_SRCS:.cpp=.o)
LOGGER_OBJS	= $(LOGGER_SRCS:.cpp=.o)

all:	$(TARGET) $(TARGET2)

$(TARGET):	$(OBJS) $(PARAMETER) $(LOGGER)
	$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS) $(PARAMETER) $(LOGGER)

$(PARAMETER):	$(PARAMETER_OBJS)
	$(AR) $(ARFLAGS) $@ $(PARAMETER_OBJS)

$(LOGGER):	$(LOGGER_OBJS)
	$(AR) $(ARFLAGS) $@ $(LOGGER_OBJS)

clean:
	rm -f $(TARGET) $(OBJS) $(PARAMETER) $(PARAMETER_OBJS) $(LOGGER) $(LOGGER_OBJS) *.log*
