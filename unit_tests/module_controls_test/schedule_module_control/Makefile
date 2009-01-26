#
#
#	Makefile for module_control unit test frameworks.
#
#
TARGET		= schedule_module_test1.so schedule_module_test2.so module_control_ut
CPP			= g++
CPPFLAGS	= -O0 -g -Wall -Werror -pthread -DMAX_BUFFER_SIZE=65535
INCLUDES	=	-I../../../logger \
				-I../../../parameter \
				-I../../../include \
				-I../../../src
LIBS		=	-lboost_system-gcc41-mt \
				-lboost_thread-gcc41-mt \
				-lboost_unit_test_framework-gcc41-mt \
				-llog4cxx

LDFLAGS		= -lrt -ldl

SRCS		=	\
			../../../src/schedule_module_control.cpp \
				schedule_module_control_test.cpp

SRC_M1		=	schedule_module_test1.cpp
SRC_M2		=	schedule_module_test2.cpp

.SUFFIX:	.cpp .o
.cpp.o:
		$(CPP) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

OBJS		= $(SRCS:.cpp=.o)

all:	$(TARGET)

schedule_module_test1.so:
	$(CPP) $(CPPFLAGS) -shared -fPIC $(INCLUDES) $(SRC_M1) $(LDFLAGS) -o schedule_module_test1.so

schedule_module_test2.so:
	$(CPP) $(CPPFLAGS) -shared -fPIC $(INCLUDES) $(SRC_M2) $(LDFLAGS) -o schedule_module_test2.so

module_control_ut: $(OBJS)
	$(CPP) $(INCLUDES) -o module_control_ut $(OBJS) $(LIBS)

#$(TARGET):	$(OBJS)
#	$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(TARGET) $(OBJS)
