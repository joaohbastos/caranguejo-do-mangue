CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -Isrc

SRC     = src/main.c src/colonia.c
OBJ     = $(SRC:.c=.o)
TARGET  = caranguejo

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
