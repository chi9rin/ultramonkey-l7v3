#
#
#	Makefile for module_control unit test frameworks.
#
#
TARGET		= virtualservice_ut
CPP			= g++
CPPFLAGS	= -O0 -g -Werror -pthread -DMAX_BUFFER_SIZE=65535
INCLUDES	=	-I../../logger \
				-I../../parameter \
				-I../../include \
				-I../../src
LIBS		=	-lboost_system-gcc41-mt \
				-lboost_thread-gcc41-mt \
				-lboost_unit_test_framework-gcc41-mt

LDFLAGS		= -lrt -ldl

SRCS		=	\
			../../src/virtualservice.cpp \
			virtualservice_test.cpp

.SUFFIX:	.cpp .o
.cpp.o:
		$(CPP) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

OBJS		= $(SRCS:.cpp=.o)

all:	$(TARGET)

$(TARGET):	$(OBJS)
	$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(TARGET) $(OBJS)
