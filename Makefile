CC = gcc
CFLAGS = -Wall -Iinclude

SRC = src/main.c \
      src/compress.c \
      src/decompress.c \
      src/fileio.c

OBJ = $(SRC:.c=.o)

compression_tool: $(OBJ)
	$(CC) $(OBJ) -o compression_tool

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) compression-tool
