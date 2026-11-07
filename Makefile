CC      = gcc
CFLAGS ?= -O2
CFLAGS += -Wall -Wextra -Iinclude
PREFIX ?= /usr/local

TARGET  = compression_tool
SRC     = $(wildcard src/*.c)
OBJ     = $(SRC:.c=.o)
DEPS    = $(wildcard include/*.h)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

install: $(TARGET)
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 $(TARGET) $(DESTDIR)$(PREFIX)/bin/

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all install clean
