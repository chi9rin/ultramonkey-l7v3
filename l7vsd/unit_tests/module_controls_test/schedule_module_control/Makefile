#
#
#	Makefile for module_control unit test frameworks.
#
#
TARGET		= sched_schedule_module_test1.so sched_schedule_module_test2.so module_control_ut
TARGET2		= sched_schedule_module_test1.so sched_schedule_module_test2.so module_control_thread_ut
CPP			= g++
CPPFLAGS	= -O0 -g -Wall -Werror -pthread -DMAX_BUFFER_SIZE=65535 -DDEBUG  -D PARAMETER_FILE='"./sample.ini"'
INCLUDES	=	-I../../../logger \
				-I../../../parameter \
				-I../../../include \
				-I../../../src
LIBS		=	-lboost_system-mt \
				-lboost_thread-mt \
				-lboost_unit_test_framework-mt \
				-llog4cxx \
				-ldl

LDFLAGS		= -lrt -ldl

#LOGGER		= libtest_logger.a
#PARAMETER	= ligtest_parameter.a
#AR			= ar
#ARFLAGS		= cru

SRCS		=	\
			../../../src/schedule_module_control.cpp \
				schedule_module_control_test.cpp

SRCS2		=	\
			../../../src/schedule_module_control.cpp \
				schedule_module_control_thread_test.cpp

SRC_M1		=	schedule_module_test1.cpp
SRC_M2		=	schedule_module_test2.cpp

#PARAMETER_SRCS	=	\
			../../../parameter/parameter.cpp \
			../../../parameter/parameter_impl.cpp

#LOGGER_SRCS	=	\
			../../../logger/logger.cpp \
			../../../logger/logger_impl.cpp \
			../../../logger/strict_time_based_rolling_policy.cpp \
			../../../logger/time_and_size_based_rolling_policy.cpp

.SUFFIX:	.cpp .o
.cpp.o:
		$(CPP) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

OBJS		= $(SRCS:.cpp=.o)
OBJS2		= $(SRCS2:.cpp=.o)

#PARAMETER_OBJS	= $(PARAMETER_SRCS:.cpp=.o)
#LOGGER_OBJS	= $(LOGGER_SRCS:.cpp=.o)

all:	$(TARGET) $(TARGET2)

sched_schedule_module_test1.so:
	$(CPP) $(CPPFLAGS) -shared -fPIC $(INCLUDES) $(SRC_M1) $(LDFLAGS) -o sched_schedule_module_test1.so

sched_schedule_module_test2.so:
	$(CPP) $(CPPFLAGS) -shared -fPIC $(INCLUDES) $(SRC_M2) $(LDFLAGS) -o sched_schedule_module_test2.so

module_control_ut: $(OBJS) $(PARAMETER) $(LOGGER)
	$(CPP) $(INCLUDES) -o module_control_ut $(OBJS) $(LIBS) $(PARAMETER) $(LOGGER)

module_control_thread_ut: $(OBJS2) $(PARAMETER) $(LOGGER)
	$(CPP) $(INCLUDES) -o module_control_thread_ut $(OBJS2) $(LIBS) $(PARAMETER) $(LOGGER)

#$(TARGET):	$(OBJS) $(PARAMETER) $(LOGGER)
#	$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS) $(PARAMETER) $(LOGGER)

#$(PARAMETER):	$(PARAMETER_OBJS)
#	$(AR) $(ARFLAGS) $@ $(PARAMETER_OBJS)

#$(LOGGER):	$(LOGGER_OBJS)
#	$(AR) $(ARFLAGS) $@ $(LOGGER_OBJS)

clean:
	rm -f $(TARGET) $(TARGET2) $(OBJS) $(OBJS2) $(PARAMETER) $(PARAMETER_OBJS) $(LOGGER) $(LOGGER_OBJS)
