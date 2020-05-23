#
# Makefile
#

CC = gcc
TARGET = evlag

CFLAGS = -Wall -pedantic -O3 -std=gnu11 $(shell pkg-config --cflags libevdev)
CFLAGS := $(CFLAGS) -I/usr/include/libevdev 
CFLAGS := $(CFLAGS) -DDEBUG
LIBS = -pthread
LIBS := $(LIBS) -levdev
LDFLAGS = $(shell pkg-config --libs libevdev)

OBJECTS = \
	main.o \
	arg_parse.o \
	fifo.o \
	threads.o

$(TARGET) : $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS) 

%.o : %.c *.h
	$(CC) $(CFLAGS)  -o $@ -c $<

clean :
	rm -f $(TARGET) *.o
