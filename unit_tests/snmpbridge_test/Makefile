#
#
#	Makefile for l7vs_command unit test frameworks.
#
#
TARGET		= l7vs_snmpbridge
TARGET2		= l7vs_snmpbridge_thread
CPP			= g++
CPPFLAGS	= -O0 -g -Wall -Werror -pthread -DMAX_BUFFER_SIZE=65535
INCLUDES	=	-I../../logger \
				-I../../parameter \
				-I../../include \
				-I../../src \
				-I../../snmpagent
LIBS		=	-lboost_system-gcc41-mt \
				-lboost_thread-gcc41-mt \
				-lboost_unit_test_framework-gcc41-mt

LDFLAGS		= -lrt -ldl

SRCS		=	\
				../../src/snmpbridge.cpp \
				snmpbridge_test.cpp

SRCS2		=	\
				../../src/snmpbridge.cpp \
				snmpbridge_thread_test.cpp

.SUFFIX:	.cpp .o
.cpp.o:
		$(CPP) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

OBJS		= $(SRCS:.cpp=.o)
OBJS2		= $(SRCS2:.cpp=.o)

all:	$(TARGET) $(TARGET2)

$(TARGET):	$(OBJS)
	$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS)

$(TARGET2):	$(OBJS2)
	$(CPP) $(INCLUDES) -o $@ $(OBJS2) $(LIBS)

clean:
	rm -f $(TARGET) $(TARGET2) $(OBJS)


