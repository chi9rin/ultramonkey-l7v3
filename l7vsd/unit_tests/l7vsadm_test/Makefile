#
#
#	Makefile for l7vsd unit test frameworks.
#
#
TARGET		= l7vsadm_ut
CPP			= g++
CPPFLAGS	= -Wall -O0 -g -Wno-write-strings -Werror -pthread -DPARAMETER_FILE='"./l7vs.cf"' -DLOGGER_PROCESS_ADM -DL7VS_CONFIG_SOCK_PATH='"."' -DMAX_BUFFER_SIZE='(65535)' -DVERSION=\"3.0.0-0\"
INCLUDES	= 	-I../../include -I../../..
LIBS		=	-lboost_system-mt \
				-lboost_thread-mt \
				-lboost_serialization-mt \
				-lboost_unit_test_framework-mt \
				-lboost_regex-mt \
				-llog4cxx 

LDFLAGS		= -lrt -ldl

LOGGER		= libtest_logger.a
PARAMETER	= ligtest_parameter.a
AR			= ar
ARFLAGS		= cru


SRCS		=	\
			stub.cpp \
			l7vsadm_test.cpp

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
PARAMETER_OBJS	= $(PARAMETER_SRCS:.cpp=.o)
LOGGER_OBJS	= $(LOGGER_SRCS:.cpp=.o)

all:	$(TARGET)

$(TARGET):	$(OBJS) $(PARAMETER) $(LOGGER)
	$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS) $(PARAMETER) $(LOGGER)

$(PARAMETER):	$(PARAMETER_OBJS)
	$(AR) $(ARFLAGS) $@ $(PARAMETER_OBJS)

$(LOGGER):	$(LOGGER_OBJS)
	$(AR) $(ARFLAGS) $@ $(LOGGER_OBJS)

clean:
	rm -f $(TARGET) $(OBJS) $(PARAMETER) $(PARAMETER_OBJS) $(LOGGER) $(LOGGER_OBJS) l7vsadm.log

