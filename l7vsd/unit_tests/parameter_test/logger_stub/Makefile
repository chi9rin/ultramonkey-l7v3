TARGET  = liblogger.a
INCDIR  = -I. -I../../../include -I../../../src
CC      = g++
CFLAGS  = -O0 -fpic -g -Wall -Werror -pthread $(INCDIR)
LD      = g++
LIBS    = -L/usr/lib64 -llog4cxx
LDFLAGS = -DPARAMETER_FILE="/home/tuka/l7vsd-3.x-ramiel/l7vsd/unit_tests/l7vs_logger/l7vs.cf"

SRCS    = logger_impl.cpp \
          strict_time_based_rolling_policy.cpp \
          logger.cpp \
          time_and_size_based_rolling_policy.cpp \

OBJS    = $(SRCS:.cpp=.o)

.SUFFIXES: .cpp .o
.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

all:    $(TARGET)

logger_impl.o:logger_impl.cpp
	$(CC) $(CFLAGS) -c $< -o $@

logger.o:logger.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

strict_time_based_rolling_policy.o:strict_time_based_rolling_policy.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

time_and_size_based_rolling_policy.o:time_and_size_based_rolling_policy.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(TARGET) $(OBJS)

$(TARGET): $(OBJS)
	ar rcs $@ $^
