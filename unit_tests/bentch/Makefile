TARGET		= atomic_bench
CPP		= g++
CPPFLAGS	= -Wall -O2 -g -Werror -pthread 
INCLUDES	=	-I../../include
LIBS		=

LDFLAGS		= -lrt -ldl


SRCS		= atomic_test.cpp


.SUFFIXES:	.o .cpp
.cpp.o:
		$(CPP) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

OBJS		= $(SRCS:.cpp=.o)

all:	$(TARGET)

$(TARGET):	$(OBJS)
	$(CPP) $(INCLUDES) -o $@ $(LDFLAGS) $(OBJS) $(LIBS)


clean:
	rm -f $(TARGET) $(OBJS)


