#
#
#	Makefile for l7vsd unit test frameworks.
#
#
TARGET		= accesslog_ut
CPP			= g++
##CPPFLAGS	= -Wall -O0 -g -Werror -pthread -D PARAMETER_FILE='"./l7vs.cf"'
CPPFLAGS	= -Wall -O0 -g -Werror -pthread -DPARAMETER_FILE='"./l7vs.cf"'
INCLUDES	=	-I../../include -I.
LIBS		=	-lboost_system-mt \
				-lboost_thread-mt \
				-lboost_unit_test_framework-mt \
				-llog4cxx 

LDFLAGS		= -lrt -ldl

LOGGER		= libtest_logger.a
PARAMETER	= ligtest_parameter.a
AR			= ar
ARFLAGS		= cru


SRCS		=	\
			../../src/parameter.cpp \
			../../src/parameter_impl.cpp \
			../../src/logger.cpp \
			../../src/logger_impl.cpp \
			../../src/logger_implement_access.cpp \
			../../src/strict_time_based_rolling_policy.cpp \
			../../src/time_and_size_based_rolling_policy.cpp \
			logger_logrotate_utility.cpp \
			logger_implement_access_test.cpp

PARAMETER_SRCS	=	\
			../../src/parameter.cpp \
			../../src/parameter_impl.cpp

#		stub.cpp \
LOGGER_SRCS	=	\
			../../src/logger.cpp \
			../../src/logger_impl.cpp \
			../../src/logger_logrotate_utility.cpp \
			../../src/logger_implement_access.cpp \
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

all:	$(TARGET)
#all:	$(TARGET) $(TARGET2)

#$(TARGET):	$(OBJS) $(PARAMETER) $(LOGGER)
#	$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS) $(PARAMETER) $(LOGGER)
$(TARGET):	$(OBJS)
	$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS)

#$(TARGET2):	$(OBJS2) $(PARAMETER) $(LOGGER)
#	$(CPP) $(INCLUDES) -o $@ $(OBJS2) $(LIBS) $(PARAMETER) $(LOGGER)

#$(PARAMETER):	$(PARAMETER_OBJS)
#	$(AR) $(ARFLAGS) $@ $(PARAMETER_OBJS)

#$(LOGGER):	$(LOGGER_OBJS)
#	$(AR) $(ARFLAGS) $(PARAMETER) $@ $(LOGGER_OBJS)

clean:
	rm -f $(TARGET) $(OBJS) $(OBJS2) $(PARAMETER) $(PARAMETER_OBJS) $(LOGGER) $(LOGGER_OBJS)

