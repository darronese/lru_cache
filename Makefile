CC = gcc
CFLAGS = -g -Wall -Werror -std=c99
INCLUDES = -Iinclude
LDFLAGS = -lm

TARGET = cachesim
SRC = src/cachesim.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET) *.o *.tmp trace.all trace.f*
