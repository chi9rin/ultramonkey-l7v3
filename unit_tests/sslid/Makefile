#
#
#	Makefile for sslid unit test frameworks.
#
#
TARGET		= protocol_module_sslid_ut
CPP			= g++
CPPFLAGS	= -g -Wall -Werror -pthread -O0 -DDATA_SIZE=480 -DMAX_BUFFER_SIZE=65535
INCLUDES	=	-I../../include \
				-I../../logger \
				-I../../module/protocol \
				-I../../src
				
LIBS		=	-lboost_system-gcc41-mt \
				-lboost_thread-gcc41-mt \
				-lboost_unit_test_framework-gcc41-mt

LDFLAGS		= -lrt -ldl

SRCS		=  protocol_module_sslid_test.cpp

OBJS		= $(SRCS:.cpp=.o)

all:	$(TARGET)

$(TARGET):	$(OBJS)
	@$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS)
	@echo "make all done!(^_^)"

clean:
	@echo "clean...(>_<)"
	@rm -f $(TARGET) $(OBJS) *.o
	@echo "clean done!"

.cpp.o:
	@$(CPP) $(CPPFLAGS) $(INCLUDES) -c $<

