#
#
#	Makefile for schedule_module unit test frameworks.
#
#
#TARGET		= schedule_module_rr.so schedule_module_ut
TARGET		= sched_schedule_module_rr.so sched_schedule_module_wrr.so sched_schedule_module_lc.so schedule_module_rr_ut schedule_module_wrr_ut  schedule_module_lc_ut
CPP			= g++
CPPFLAGS	= -O0 -g -Wall -pthread -DMAX_BUFFER_SIZE=65535 -D PARAMETER_FILE='"./sample.ini"'
INCLUDES	=	-I../../logger \
				-I../../parameter \
				-I../../include \
				-I../../module/schedule
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
				schedule_module_rr_test.cpp \
				schedule_module_wrr_test.cpp \
				schedule_module_lc_test.cpp

SRCS_RR		=	\
				schedule_module_rr_test.cpp \
				../../src/schedule_module_control.cpp

SRCS_WRR		=	\
				schedule_module_wrr_test.cpp \
				../../src/schedule_module_control.cpp

SRCS_LC		=	\
				schedule_module_lc_test.cpp \
				../../src/schedule_module_control.cpp

SRC_M1		=	../../module/schedule/schedule_module_rr.cpp

SRC_M2		=	../../module/schedule/schedule_module_wrr.cpp

SRC_M3		=	../../module/schedule/schedule_module_lc.cpp

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

OBJS_RR		= $(SRCS_RR:.cpp=.o)

OBJS_WRR	= $(SRCS_WRR:.cpp=.o)

OBJS_LC		= $(SRCS_LC:.cpp=.o)

PARAMETER_OBJS	= $(PARAMETER_SRCS:.cpp=.o)
LOGGER_OBJS	= $(LOGGER_SRCS:.cpp=.o)

all:	$(TARGET)

sched_schedule_module_rr.so: $(SRC_M1)
	$(CPP) $(CPPFLAGS) -shared -fPIC $(INCLUDES) $(SRC_M1) $(LDFLAGS) -o sched_schedule_module_rr.so

sched_schedule_module_wrr.so: $(SRC_M2)
	$(CPP) $(CPPFLAGS) -shared -fPIC $(INCLUDES) $(SRC_M2) $(LDFLAGS) -o sched_schedule_module_wrr.so

sched_schedule_module_lc.so: $(SRC_M3)
	$(CPP) $(CPPFLAGS) -shared -fPIC $(INCLUDES) $(SRC_M3) $(LDFLAGS) -o sched_schedule_module_lc.so

schedule_module_rr_ut: $(OBJS_RR) $(SRC_M1)
	$(CPP) $(INCLUDES) -o schedule_module_rr_ut $(OBJS_RR) $(LIBS)

schedule_module_wrr_ut: $(OBJS_WRR) $(SRC_M2)
	$(CPP) $(INCLUDES) -o schedule_module_wrr_ut $(OBJS_WRR) $(LIBS)

schedule_module_lc_ut: $(OBJS_LC) $(SRC_M3)
	$(CPP) $(INCLUDES) -o schedule_module_lc_ut $(OBJS_LC) $(LIBS)

#schedule_module_rr_ut: $(OBJS_RR) $(SRC_M1) $(PARAMETER) $(LOGGER)
#	$(CPP) $(INCLUDES) -o schedule_module_rr_ut $(OBJS_RR) $(LIBS) $(PARAMETER) $(LOGGER)

#schedule_module_wrr_ut: $(OBJS_WRR) $(SRC_M2) $(PARAMETER) $(LOGGER)
#	$(CPP) $(INCLUDES) -o schedule_module_wrr_ut $(OBJS_WRR) $(LIBS) $(PARAMETER) $(LOGGER)

#schedule_module_lc_ut: $(OBJS_LC) $(SRC_M3) $(PARAMETER) $(LOGGER)
#	$(CPP) $(INCLUDES) -o schedule_module_lc_ut $(OBJS_LC) $(LIBS) $(PARAMETER) $(LOGGER)

#$(PARAMETER):	$(PARAMETER_OBJS)
#	$(AR) $(ARFLAGS) $@ $(PARAMETER_OBJS)

#$(LOGGER):	$(LOGGER_OBJS)
#	$(AR) $(ARFLAGS) $@ $(LOGGER_OBJS)

#$(TARGET):	$(OBJS) $(PARAMETER) $(LOGGER)
#	$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS)  $(PARAMETER) $(LOGGER)

clean:
	rm -f $(TARGET) $(OBJS) $(PARAMETER) $(PARAMETER_OBJS) $(LOGGER) $(LOGGER_OBJS)
