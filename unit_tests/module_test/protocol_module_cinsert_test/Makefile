#
#
#	Makefile for protocol_module_cinsert unit test frameworks.
#
#
TARGET		= protocol_module_cinsert_ut protocol_module_cinsert3_ut protocol_module_cinsert4_ut
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
			../../../module/protocol/http_protocol_module_base.cpp \
			../../../module/protocol/protocol_module_cinsert.cpp \
			protocol_module_cinsert_test_1.cpp

SRCS3		=	\
			../../../module/protocol/http_protocol_module_base.cpp \
			../../../module/protocol/protocol_module_cinsert.cpp \
			protocol_module_cinsert_test_3.cpp

SRCS4		=	\
			../../../module/protocol/http_protocol_module_base.cpp \
			../../../module/protocol/protocol_module_cinsert.cpp \
			protocol_module_cinsert_test_4.cpp

.SUFFIX:	.cpp .o
.cpp.o:
		$(CPP) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

OBJS		= $(SRCS:.cpp=.o)

OBJS3		= $(SRCS3:.cpp=.o)

OBJS4		= $(SRCS4:.cpp=.o)

all:	$(TARGET)

protocol_module_cinsert_ut:	$(OBJS)
	$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS)

protocol_module_cinsert3_ut:	$(OBJS3)
	$(CPP) $(INCLUDES) -o $@ $(OBJS3) $(LIBS)

protocol_module_cinsert4_ut:	$(OBJS4)
	$(CPP) $(INCLUDES) -o $@ $(OBJS4) $(LIBS)

clean:
	rm -f $(TARGET) $(OBJS) $(OBJS3) $(OBJS4)
