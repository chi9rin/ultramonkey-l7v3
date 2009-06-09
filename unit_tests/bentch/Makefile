TARGET		= atomic_bench
TARGET2		= queue_bench
CPP		= g++
CPPFLAGS	= -O2 -g -Werror -pthread 
INCLUDES	=	-I../../include
LIBS		=

LDFLAGS		= -lrt -ldl -pthread


SRCS		= atomic_test.cpp
SRCS2		= queue_test.cpp


.SUFFIXES:	.o .cpp
.cpp.o:
		$(CPP) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

OBJS		= $(SRCS:.cpp=.o)
OBJS2		= $(SRCS2:.cpp=.o)

all:	$(TARGET) $(TARGET2)

$(TARGET):	$(OBJS)
	$(CPP) $(INCLUDES) -o $@ $(LDFLAGS) $(OBJS) $(LIBS)

$(TARGET2):	$(OBJS2)
	$(CPP) $(INCLUDES) -o $@ $(LDFLAGS) $(OBJS2) $(LIBS)


clean:
	rm -f $(TARGET) $(OBJS) $(TARGET2) $(OBJS2)


