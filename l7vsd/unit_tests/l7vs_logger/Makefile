TARGET	= logger_test
INCDIR	= ../../sachiel/parameter/
SRCDIR	= ../../sachiel/logger/
CC	= g++
CFLAGS	= -O0 -g -Wall -Werror -pthread 
LD	= g++
LIBS	= -L/usr/lib64 -lrt
LDFLAGS	= 

SRCS	= \
	  logger_test.cpp \
	  log4cxx/level.cpp

.SUFFIXES: .cpp .o
.cpp.o:
	$(CC) $(CFLAGS) -I$(INCDIR) -I$(SRCDIR) -c $< -o $@

OBJS	= $(SRCS:.cpp=.o)

all:	$(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) $(LIBS) -o $(TARGET) $(OBJS) /usr/lib64/libboost_unit_test_framework.a

logger_test.o: $(SRCDIR)logger_impl.h $(SRCDIR)logger_impl.cpp

clean:
	rm -f $(TARGET) *.o
