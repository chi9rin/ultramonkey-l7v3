#
#
#	Makefile for sslid unit test frameworks.
#
#
TARGET		= sslid_ut
CPP			= g++
CPPFLAGS	= -g -Wall -Wno-write-strings -Werror -pthread -O0 -DMAX_BUFFER_SIZE=65535
INCLUDES	=	-I../../../include \
				-I../../../logger \
				-I../../../module/protocol
				
LIBS		=	-lboost_system-mt \
				-lboost_thread-mt \
				-lboost_unit_test_framework-mt

LDFLAGS		= -lrt -ldl

SRCS		=  sslid_ut.cpp

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

