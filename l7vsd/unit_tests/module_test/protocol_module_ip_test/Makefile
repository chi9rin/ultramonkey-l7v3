#
#
#	Makefile for protocol_module_sessionless unit test frameworks.
#
#
TARGET		= ip_ut
CPP			= g++
CPPFLAGS	= -g -Wall -Werror -Wno-write-strings -pthread -O0 -DMAX_BUFFER_SIZE=65535u
INCLUDES	=	-I../../../logger \
				-I../../../parameter \
				-I../../../include \
				-I../../../src
LIBS		=	-lboost_system-mt \
				-lboost_thread-mt \
				-lboost_regex-mt \
				-lboost_unit_test_framework-mt

LDFLAGS		= -lrt -ldl

SRCS		=	ip_ut.cpp

OBJS		= $(SRCS:.cpp=.o)

all:	$(TARGET)

$(TARGET):	$(OBJS)
	@$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS)
	@echo "make all done!(^_^)"

clean:
	@echo "clean...(>_<)"
	@rm -f $(TARGET) $(OBJS)
	@echo "clean done!"

.cpp.o:
	@$(CPP) $(CPPFLAGS) $(INCLUDES) -c $<

