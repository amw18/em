CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -g
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
TARGET = usb_device_sim

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
