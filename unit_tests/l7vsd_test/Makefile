#
#
#	Makefile for l7vsd unit test frameworks.
#
#
TARGET		= l7vsd_ut
CPP			= g++
CPPFLAGS	= -O0 -g -Werror -pthread
INCLUDES	=	-I../../logger \
				-I../../parameter \
				-I../../include
LIBS		=	-lboost_system-gcc41-mt \
				-lboost_thread-gcc41-mt \
				-lboost_unit_test_framework-gcc41-mt

LDFLAGS		= -lrt -ldl

SRCS		=	\
			l7vsd_test.cpp

.SUFFIX:	.cpp .o
.cpp.o:
		$(CPP) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

OBJS		= $(SRCS:.cpp=.o)

all:	$(TARGET)

$(TARGET):	$(OBJS)
	$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS) /usr/lib64/libboost_unit_test_framework-gcc41-mt.a

clean:
	rm -f $(TARGET) $(OBJS)
