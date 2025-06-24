CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -g
SRC = $(wildcard src/*.c) config/usb_desc.c
OBJ = $(SRC:.c=.o)
TARGET = eub_monitor

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lusb-1.0

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
