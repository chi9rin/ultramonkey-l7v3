#
#
#	Makefile for module_control unit test frameworks.
#
#
TARGET		= protomod_protocol_module_test1.so protomod_protocol_module_test2.so module_control_ut
CPP			= g++
CPPFLAGS	= -O0 -g -Wall -Werror -pthread -DMAX_BUFFER_SIZE=65535
INCLUDES	=	-I../../../logger \
				-I../../../parameter \
				-I../../../include \
				-I../../../src
LIBS		=	-lboost_system-gcc41-mt \
				-lboost_thread-gcc41-mt \
				-lboost_unit_test_framework-gcc41-mt \
				-ldl

LDFLAGS		= -lrt -ldl

SRCS		=	\
			../../../src/protocol_module_control.cpp \
				protocol_module_control_test.cpp

SRC_M1		=	protocol_module_test1.cpp
SRC_M2		=	protocol_module_test2.cpp

.SUFFIX:	.cpp .o
.cpp.o:
		$(CPP) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

OBJS		= $(SRCS:.cpp=.o)

all:	$(TARGET)

protomod_protocol_module_test1.so:
	$(CPP) $(CPPFLAGS) -shared -fPIC $(INCLUDES) $(SRC_M1) $(LDFLAGS) -o protomod_protocol_module_test1.so

protomod_protocol_module_test2.so:
	$(CPP) $(CPPFLAGS) -shared -fPIC $(INCLUDES) $(SRC_M2) $(LDFLAGS) -o protomod_protocol_module_test2.so

module_control_ut: $(OBJS)
	$(CPP) $(INCLUDES) -o module_control_ut $(OBJS) $(LIBS)

#$(TARGET):	$(OBJS)
#	$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(TARGET) $(OBJS)
