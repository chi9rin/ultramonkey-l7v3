#
#
#	Makefile for tcp_session_test Class unit test frameworks.
#
#
TARGET		= tcp_session_test_ut
CPP			= g++
CPPFLAGS	= -O0 -g -Werror -Wall -pthread -DMAX_BUFFER_SIZE=65535 -DDEBUG -D PARAMETER_FILE='"./l7vs.cf"'
INCLUDES	=		-I/usr/include \
				-I./ \
				-I../logger_stb \
				-I../../../src \
				-I../../../include
LIBS		=	-lboost_system-gcc41-mt \
				-lboost_thread-gcc41-mt \
				-lboost_unit_test_framework-gcc41-mt \
				-lssl

LDFLAGS		= -lrt -ldl

PARAMETER	= libtest_parameter.a
AR			= ar
ARFLAGS		= cru

PARAMETER_SRCS	=	\
			../../../src/parameter.cpp \
			../../../src/parameter_impl.cpp

SRCS		=	\
			../logger_stb/logger.cpp \
			../logger_stb/logger_implement_access.cpp \
			tcp_session_test.cpp
			
.SUFFIX:	.cpp .o
.cpp.o:
		$(CPP) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

OBJS		= $(SRCS:.cpp=.o)
PARAMETER_OBJS	= $(PARAMETER_SRCS:.cpp=.o)

all:	$(TARGET)

$(TARGET):	$(OBJS) $(PARAMETER)
	$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS) $(PARAMETER)
#$(TARGET):      $(OBJS)
#	$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS)

$(PARAMETER):	$(PARAMETER_OBJS)
	$(AR) $(ARFLAGS) $@ $(PARAMETER_OBJS)

clean:
	rm -f $(TARGET) $(OBJS) $(PARAMETER) $(PARAMETER_OBJS)
#	rm -f $(TARGET) $(OBJS)

