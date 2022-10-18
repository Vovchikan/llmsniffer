TARGET := llmsniffer
LIBS := -lm

ifeq ($(ARCH),)
	ARCH := noarm
endif

ifeq ($(ARCH), arm)
	CC := arm-linux-gnueabi-gcc
else
	CC := gcc
endif

CFLAGS := -g -Wall

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS := $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS := $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)
