#
#
#	Makefile for l7vsd unit test frameworks.
#
#
LOGGER		= libtest_logger.a
PARAMETER	= ligtest_parameter.a
AR			= ar
ARFLAGS		= cru
TARGET		= l7vsd_ut
CPP			= g++
CPPFLAGS	= -O0 -g -Werror -pthread
INCLUDES	=	-I../../logger \
				-I../../parameter \
				-I../../include
LIBS		=	-lboost_system-gcc41-mt \
				-lboost_thread-gcc41-mt \
				-lboost_unit_test_framework-gcc41-mt \
				-llog4cxx 

LDFLAGS		= -lrt -ldl

SRCS		=	\
			l7vsd_test.cpp

PARAMETER_SRCS	=	\
			../../parameter/parameter_enum.h \
			../../parameter/parameter.h \
			../../parameter/parameter_impl.h \
			../../parameter/parameter.cpp \
			../../parameter/parameter_impl.cpp

LOGGER_SRCS	=	\
			../../logger_enum.h \
			../../logger_rotation_enum.h \
			../../logger.h \
			../../logger_impl.h \
			../../logger.cpp \
			../../logger_impl.cpp \
			../../strict_time_based_rolling_policy.h \
			../../strict_time_based_rolling_policy.cpp \
			../../time_and_size_based_rolling_policy.h \
			../../time_and_size_based_rolling_policy.cpp

.SUFFIX:	.cpp .o
.cpp.o:
		$(CPP) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

OBJS		= $(SRCS:.cpp=.o)
PARAMETER_OBJS	= $(PARAMETER_SRCS:.cpp=.o)
LOGGER_OBJS	= $(LOGGER_SRCS:.cpp=.o)

all:	$(TARGET)

$(TARGET):	$(OBJS)
	$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS) $(PARAMETER) $(LOGGER)

$(PARAMETER):	$(PARAMETER_OBJS)
	$(AR) $(ARFLAGS) -o $@ $(PARAMETER_OBJS)

$(LOGGER):	$(LOGGER_OBJS)
	$(AR) $(ARFLAGS) -o $@ $(LOGGER_OBJS)

clean:
	rm -f $(TARGET) $(OBJS)
