#
#
#	Makefile for protocol_module_cinsert unit test frameworks.
#
#
TARGET		= protocol_module_cinsert_ut
CPP			= g++
CPPFLAGS	= -O0 -g -Werror -Wall -pthread -DMAX_BUFFER_SIZE=65535 -DDEBUG
INCLUDES	=	-I../../../logger \
				-I../../../parameter \
				-I../../../include \
				-I../../../module/protocol \
				-I../../../src
LIBS		=	-lboost_system-gcc41-mt \
				-lboost_thread-gcc41-mt \
				-lboost_regex-gcc41-mt \
				-lboost_unit_test_framework-gcc41-mt

LDFLAGS		= -lrt -ldl

SRCS		=	\
			../../../module/protocol/protocol_module_cinsert.cpp \
			protocol_module_cinsert_test.cpp


.SUFFIX:	.cpp .o
.cpp.o:
		$(CPP) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

OBJS		= $(SRCS:.cpp=.o)

all:	$(TARGET)

$(TARGET):	$(OBJS)
	$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(TARGET) $(OBJS)
