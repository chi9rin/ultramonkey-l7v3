TARGET		= atomic_test

##TARGET2		= queue_bench
##TARGET3		= hash_bench
CPP		= g++
CPPFLAGS	= -O2 -g -Werror -pthread -D__X86_64__
INCLUDES	=	-I../../logger \
				-I../../parameter \
				-I./ \
				-I../../include \
				-I../../src \
				-I../../snmpagent  \
                                -I/usr/include
#LIBS		=
LIBS		=	-lboost_system-mt \
				-lboost_thread-mt \
				-lboost_unit_test_framework-mt \
				-lboost_serialization-mt



LDFLAGS		= -lrt -ldl -pthread


SRCS		= atomic_test.cpp
##SRCS2		= queue_test.cpp
##SRCS3		= hashmap_test.cpp



.SUFFIXES:	.o .cpp
.cpp.o:
		$(CPP) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

OBJS		= $(SRCS:.cpp=.o)
OBJS2		= $(SRCS2:.cpp=.o)
OBJS3		= $(SRCS3:.cpp=.o)

all:	$(TARGET) $(TARGET2) $(TARGET3)

$(TARGET):	$(OBJS)
	$(CPP) $(INCLUDES) -o $@ $(LDFLAGS) $(OBJS) $(LIBS)

$(TARGET2):	$(OBJS2)
	$(CPP) $(INCLUDES) -o $@ $(LDFLAGS) $(OBJS2) $(LIBS)


$(TARGET3):	$(OBJS3)
	$(CPP) $(INCLUDES) -o $@ $(LDFLAGS) $(OBJS3) $(LIBS)

clean:
	rm -f $(TARGET) $(OBJS) $(TARGET2) $(OBJS2) $(TARGET3) $(OBJS3)


