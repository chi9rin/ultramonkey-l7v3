TARGET		= l7vsd_threadtest
CPP		= g++
CPPFLAGS	= -O2 -g -Werror -pthread -D__X86_64__
INCLUDES	=	-I../include
LIBS		=	-lboost_system-gcc41-mt \
				-lboost_thread-gcc41-mt \
				-lboost_unit_test_framework-gcc41-mt

LDFLAGS		= -lrt -ldl

SRCS		= l7vsd_threadtest.cpp

.SUFFIXES:	.o .cpp
.cpp.o:
		$(CPP) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

OBJS		= $(SRCS:.cpp=.o)

all:	$(TARGET)

$(TARGET):	$(OBJS)
	$(CPP) $(INCLUDES) -o $@ $(LDFLAGS) $(OBJS) $(LIBS)
	rm -f $(OBJS)
clean:
	rm -f $(TARGET) $(OBJS)


