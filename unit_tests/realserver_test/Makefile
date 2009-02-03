#
#
#	Makefile for realserver unit test frameworks.
#
#
TARGET		= realserver_ut
CPP			= g++
CPPFLAGS	= -O0 -g -Wall -pthread -DMAX_BUFFER_SIZE=65535 -D PARAMETER_FILE='"./sample.ini"'
INCLUDES	=	-I../../logger \
				-I../../parameter \
				-I../../include
LIBS		=	-lboost_system-gcc41-mt \
				-lboost_thread-gcc41-mt \
				-lboost_unit_test_framework-gcc41-mt \
				-llog4cxx 

LDFLAGS		= -lrt -ldl

LOGGER		= libtest_logger.a
PARAMETER	= libtest_parameter.a
AR			= ar
ARFLAGS		= cru

SRCS		=	\
			realserver_test.cpp

PARAMETER_SRCS	=	\
			../../parameter/parameter.cpp \
			../../parameter/parameter_impl.cpp

LOGGER_SRCS	=	\
			../../logger/logger.cpp \
			../../logger/logger_impl.cpp \
			../../logger/strict_time_based_rolling_policy.cpp \
			../../logger/time_and_size_based_rolling_policy.cpp

.SUFFIX:	.cpp .o
.cpp.o:
		$(CPP) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

OBJS		= $(SRCS:.cpp=.o)
PARAMETER_OBJS	= $(PARAMETER_SRCS:.cpp=.o)
LOGGER_OBJS	= $(LOGGER_SRCS:.cpp=.o)

all:	$(TARGET)

$(TARGET):	$(OBJS)
	$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS)

#$(TARGET):	$(OBJS) $(PARAMETER) $(LOGGER)
#	$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS) $(PARAMETER) $(LOGGER)

#$(PARAMETER):	$(PARAMETER_OBJS)
#	$(AR) $(ARFLAGS) $@ $(PARAMETER_OBJS)

#$(LOGGER):	$(LOGGER_OBJS)
#	$(AR) $(ARFLAGS) $@ $(LOGGER_OBJS)

clean:
	rm -f $(TARGET) $(OBJS) $(PARAMETER) $(PARAMETER_OBJS) $(LOGGER) $(LOGGER_OBJS)
