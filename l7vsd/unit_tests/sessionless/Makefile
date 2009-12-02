#
#
#	Makefile for protocol_module_sessionless unit test frameworks.
#
#
TARGET		= sessionless_ut
CPP			= g++
CPPFLAGS	= -g -Wall -Werror -pthread -O0 -DMAX_BUFFER_SIZE=65535u
INCLUDES	=	-I../../logger \
				-I../../parameter \
				-I../../include \
				-I../../src
LIBS		=	-lboost_system-gcc41-mt \
				-lboost_thread-gcc41-mt \
				-lboost_regex-gcc41-mt \
				-lboost_unit_test_framework-gcc41-mt

LDFLAGS		= -lrt -ldl

SRCS		=	sessionless_ut.cpp

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

