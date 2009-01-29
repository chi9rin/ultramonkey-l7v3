TARGET		=	sessionless_ut
CPP			=	g++
INCLUDES	= 
CPPFLAGS	=	-g -Wall -Werror -pthread -O0
LD			=	g++
LIBS		=	-lboost_system_gcc41-mt \
				-lboost_thread_gcc41-mt
LDFLAGS		=	-lrt -ldl -pthread
SRCS		=	sessionless_ut.cpp
OBJS		=	$(SRCS:.cpp=.o)

all:	$(TARGET)

$(TARGET):	$(OBJS)
	$(LD) $(INCLUDES) $(LDFLAGS) -o -$@ $(OBJS) $(LIBS)

clean:
	rm -f $(TARGET) $(OBJS)

.cpp.o:
	$(CPP) $(CPPFLAGS) $(INCLUDES) -c $<
