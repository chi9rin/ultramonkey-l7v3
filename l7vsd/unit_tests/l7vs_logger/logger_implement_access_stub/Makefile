TARGET	= logger_implement_access.a
INCDIR	= .
CC	= g++
CFLAGS	= -O0 -fpic -g -Wall -Werror -pthread -I$(INCDIR)
LD	= g++
LIBS	= -L/usr/lib64
LDFLAGS	= -DPARAMETER_FILE="/home/tuka/l7vsd-3.x-ramiel/l7vsd/unit_tests/l7vs_logger/l7vs.cf"

SRCS	= logger_implement_access.cpp

#SRCS	= logger_implement_access.cpp

OBJS	= $(SRCS:.cpp=.o)

.SUFFIXES: .cpp .o
.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

all:	$(TARGET)

parameter_impl.o:logger_implement_access.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

$(TARGET): $(OBJS)
	ar rcs $@ $^
