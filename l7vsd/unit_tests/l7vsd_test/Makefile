#
#
#	Makefile for l7vsd unit test frameworks.
#
#
TARGET		= l7vsd_ut
TARGET2		= l7vsd_thread
CPP			= g++
CPPFLAGS	= -Wall -O0 -g -Werror -pthread -D PARAMETER_FILE='"./l7vs.cf"'
INCLUDES	=		-I. \
				-I../../include
LIBS		=	-lboost_system-gcc41-mt \
				-lboost_thread-gcc41-mt \
				-lboost_unit_test_framework-gcc41-mt \
				-llog4cxx 

LDFLAGS		= -lrt -ldl -L.

LOGGER		= libtest_logger.a
PARAMETER	= ligtest_parameter.a
AR			= ar
ARFLAGS		= cru


SRCS		=	\
			stub.cpp \
			l7vsd_test.cpp

SRCS2		=	\
			stub.cpp \
			l7vsd_thread_test.cpp


PARAMETER_SRCS	=	\
			../../src/parameter.cpp \
			../../src/parameter_impl.cpp

LOGGER_SRCS	=	\
			../../src/logger.cpp \
			../../src/logger_impl.cpp \
			../../src/strict_time_based_rolling_policy.cpp \
			../../src/time_and_size_based_rolling_policy.cpp

.SUFFIXES:	.o .cpp
.cpp.o:
		$(CPP) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

OBJS		= $(SRCS:.cpp=.o)
OBJS2		= $(SRCS2:.cpp=.o)
PARAMETER_OBJS	= $(PARAMETER_SRCS:.cpp=.o)
LOGGER_OBJS	= $(LOGGER_SRCS:.cpp=.o)

all:	$(TARGET) $(TARGET2)

$(TARGET):	$(OBJS) $(PARAMETER) $(LOGGER)
	$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS) $(PARAMETER) $(LOGGER)

$(TARGET2):	$(OBJS2) $(PARAMETER) $(LOGGER)
	$(CPP) $(INCLUDES) -o $@ $(OBJS2) $(LIBS) $(PARAMETER) $(LOGGER)

$(PARAMETER):	$(PARAMETER_OBJS)
	$(AR) $(ARFLAGS) $@ $(PARAMETER_OBJS)

$(LOGGER):	$(LOGGER_OBJS)
	$(AR) $(ARFLAGS) $@ $(LOGGER_OBJS)

clean:
	rm -f $(TARGET) $(TARGET2) $(OBJS) $(OBJS2) $(PARAMETER) $(PARAMETER_OBJS) $(LOGGER) $(LOGGER_OBJS)

