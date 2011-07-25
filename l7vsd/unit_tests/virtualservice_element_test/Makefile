#
#
#	Makefile for vselement unit test frameworks.
#
#
TARGET		= vselement_ut
CPP			= g++
CPPFLAGS	= -Wall -O0 -g -Werror -pthread
INCLUDES	=	-I../../include
LIBS		=	-lboost_system-mt \
				-lboost_thread-mt \
				-lboost_serialization-mt \
				-lboost_unit_test_framework-mt \

LDFLAGS		= -lrt -ldl

AR			= ar
ARFLAGS		= cru

SRCS		=	\
			vselement_test.cpp

.SUFFIX:	.cpp .o
.cpp.o:
		$(CPP) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

OBJS		= $(SRCS:.cpp=.o)

all:	$(TARGET)

$(TARGET):	$(OBJS)
	$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(TARGET) $(OBJS) save.dat
