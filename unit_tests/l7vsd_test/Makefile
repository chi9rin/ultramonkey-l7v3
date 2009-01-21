#
#
#	Makefile for l7vsd unit test frameworks.
#
#
TARGET		= l7vsd_ut
CPP			= g++
CPPFLAGS	= -O0 -g -Werror -pthread
INCLUDES	=	-I../../logger \
				-I../../parameter \
				-I../../include
LIBS		=	-lboost_system-gcc41-mt \
				-lboost_thread-gcc41-mt \
				-lboost_unit_test_framework-gcc41-mt \
				-llog4cxx \
				../../logger/libl7vsd_logger.a \
				../../parameter/libl7vsd_parameter.a

LDFLAGS		= -lrt -ldl

SRCS		=	\
			l7vsd_test.cpp

.SUFFIX:	.cpp .o
.cpp.o:
		$(CPP) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

OBJS		= $(SRCS:.cpp=.o)

all:	$(TARGET)

$(TARGET):	$(OBJS)
	$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(TARGET) $(OBJS)
