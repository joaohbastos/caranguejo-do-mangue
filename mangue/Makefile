CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -Iinclude
LDFLAGS = -lraylib -lm

# Ajuste o caminho do raylib conforme sua instalação
# macOS (brew):    LDFLAGS = -lraylib -lm -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
# Linux:           LDFLAGS = -lraylib -lm -lpthread -ldl -lrt -lX11
# Windows (MinGW): LDFLAGS = -lraylib -lopengl32 -lgdi32 -lwinmm

SRC = src/main.c src/fila.c src/logica.c src/render.c
OBJ = $(SRC:.c=.o)
TARGET = colonia_do_mangue

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
