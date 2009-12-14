#
#
#	Makefile for protocol_module_base unit test frameworks.
#
#
TARGET		= protocol_module_base_ut
CPP			= g++
CPPFLAGS	= -g -Wall -Werror -pthread -O0 -DMAX_BUFFER_SIZE=65535u
INCLUDES	=	-I../../../include 
LIBS		=	-lboost_system-gcc41-mt \
				-lboost_unit_test_framework-gcc41-mt

LDFLAGS		= -lrt -ldl

SRCS		=	protocol_module_base_ut.cpp

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

