#
#
#	Makefile for schedule_module unit test frameworks.
#
#
#TARGET		= schedule_module_rr.so schedule_module_ut
TARGET		= schedule_module_rr.so schedule_module_wrr.so schedule_module_lc.so schedule_module_rr_ut schedule_module_wrr_ut
CPP			= g++
CPPFLAGS	= -O0 -g -Wall -Werror -pthread -DMAX_BUFFER_SIZE=65535
INCLUDES	=	-I../../logger \
				-I../../parameter \
				-I../../include \
				-I../../src \
				-I../../module/schedule
LIBS		=	-lboost_system-gcc41-mt \
				-lboost_thread-gcc41-mt \
				-ldl \
				-lboost_unit_test_framework-gcc41-mt

LDFLAGS		= -lrt -ldl

SRCS		=	\
				schedule_module_rr_test.cpp \
				schedule_module_wrr_test.cpp \
				schedule_module_lc_test.cpp

SRCS_RR		=	\
				schedule_module_rr_test.cpp \
				../../src/realserver.cpp

SRCS_WRR		=	\
				schedule_module_wrr_test.cpp \
				../../src/realserver.cpp

SRCS_LC		=	\
				schedule_module_lc_test.cpp \
				../../src/realserver.cpp

SRC_M1		=	../../module/schedule/schedule_module_rr.cpp

SRC_M2		=	../../module/schedule/schedule_module_wrr.cpp

SRC_M3		=	../../module/schedule/schedule_module_lc.cpp

.SUFFIX:	.cpp .o
.cpp.o:
		$(CPP) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

OBJS		= $(SRCS:.cpp=.o)

OBJS_RR		= $(SRCS_RR:.cpp=.o)

OBJS_WRR	= $(SRCS_WRR:.cpp=.o)

OBJS_LC		= $(SRCS_LC:.cpp=.o)

all:	$(TARGET)

schedule_module_rr.so: $(SRC_M1)
	$(CPP) $(CPPFLAGS) -shared -fPIC $(INCLUDES) $(SRC_M1) $(LDFLAGS) -o schedule_module_rr.so

schedule_module_wrr.so: $(SRC_M2)
	$(CPP) $(CPPFLAGS) -shared -fPIC $(INCLUDES) $(SRC_M2) $(LDFLAGS) -o schedule_module_wrr.so

schedule_module_lc.so: $(SRC_M3)
	$(CPP) $(CPPFLAGS) -shared -fPIC $(INCLUDES) $(SRC_M3) $(LDFLAGS) -o schedule_module_lc.so

schedule_module_rr_ut: $(OBJS_RR) $(SRC_M1)
	$(CPP) $(INCLUDES) -o schedule_module_rr_ut $(OBJS_RR) $(LIBS)

schedule_module_wrr_ut: $(OBJS_WRR) $(SRC_M2)
	$(CPP) $(INCLUDES) -o schedule_module_wrr_ut $(OBJS_WRR) $(LIBS)

#schedule_module_lc_ut: $(OBJS_LC) $(SRC_M3)
#	$(CPP) $(INCLUDES) -o schedule_module_lc_ut $(OBJS_LC) $(LIBS)

#schedule_module_ut: $(OBJS)
#	$(CPP) $(INCLUDES) -o schedule_module_ut $(OBJS) $(LIBS)

#$(TARGET):	$(OBJS)
#	$(CPP) $(INCLUDES) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(TARGET) $(OBJS)
