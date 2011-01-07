TARGET	= libparameter.a
INCDIR	= -I. -I../../../include -I../../../src
CC	= g++
CFLAGS	= -O0 -fpic -g -Wall -Werror -pthread $(INCDIR)
LD	= g++
LIBS	= -L/usr/lib64
LDFLAGS	= -DPARAMETER_FILE="/home/tuka/l7vsd-3.x-ramiel/l7vsd/unit_tests/l7vs_logger/l7vs.cf"

SRCS	= parameter_impl.cpp \
	  parameter.cpp 
#SRCS	= parameter_impl.cpp

OBJS	= $(SRCS:.cpp=.o)

.SUFFIXES: .cpp .o
.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

all:	$(TARGET)

parameter_impl.o:parameter_impl.cpp
	$(CC) $(CFLAGS) -c $< -o $@

parameter.o:parameter.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(TARGET) $(OBJS)

$(TARGET): $(OBJS)
	ar rcs $@ $^
