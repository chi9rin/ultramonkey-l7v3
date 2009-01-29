TARGET		=	sslid_base_ut
TARGET2		=	sslid_ut
TARGET3		=	sslid_replication_data_processor_ut
TARGET4		=	sslid_session_data_processor_ut
CPP			=	g++
INCLUDES	= 
CPPFLAGS	=	-g -Wall -Werror -pthread -O0
LD			=	g++
LIBS		=	-lboost_system_gcc41-mt \
				-lboost_thread_gcc41-mt
LDFLAGS		=	-lrt -ldl -pthread
SRCS		=	protocol_module_sslid_test.cpp
SRCS2		=	ssl_protocol_module_base_test.cpp
SRCS3		=	sslid_replication_data_processor_test.cpp
SRCS4		=	sslid_session_data_processor_test.cpp
OBJS		=	$(SRCS:.cpp=.o)
OBJS2		=	$(SRCS2:.cpp=.o)
OBJS3		=	$(SRCS3:.cpp=.o)
OBJS4		=	$(SRCS4:.cpp=.o)

all:	$(TARGET) $(TARGET2) $(TARGET3) $(TARGET4)

$(TARGET):	$(OBJS)
	$(LD) $(INCLUDES) $(LDFLAGS) -o -$@ $(OBJS) $(LIBS)

$(TARGET2):	$(OBJS2)
	$(LD) $(INCLUDES) $(LDFLAGS) -o -$@ $(OBJS2) $(LIBS)

$(TARGET3):	$(OBJS3)
	$(LD) $(INCLUDES) $(LDFLAGS) -o -$@ $(OBJS3) $(LIBS)

$(TARGET4):	$(OBJS4)
	$(LD) $(INCLUDES) $(LDFLAGS) -o -$@ $(OBJS4) $(LIBS)

clean:
	rm -f $(TARGET) $(TARGET2) $(TARGET3) $(TARGET4) $(OBJS) $(OBJS2) $(OBJS3) $(OBJS4)

.cpp.o:
	$(CPP) $(CPPFLAGS) $(INCLUDES) -c $<
