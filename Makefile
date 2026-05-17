CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -Isrc

SRC_TERMINAL = src/main.c \
               src/colonia.c \
               src/eventos.c \
               src/game_state.c \
               src/jogo.c \
               src/ranking.c \
               src/render_terminal.c
OBJ_TERMINAL = $(SRC_TERMINAL:.c=.o)
TARGET_TERMINAL = caranguejo

# GUI build (raylib). Link flags below are for MinGW on Windows;
# swap for your platform if you ever build elsewhere:
#   Linux:   -lraylib -lm -lpthread -ldl -lrt -lX11
#   macOS:   -lraylib -lm -framework CoreVideo -framework IOKit -framework Cocoa -framework OpenGL
SRC_GUI = src/main_gui.c \
          src/game_state.c \
          src/colonia.c \
          src/jogo.c \
          src/eventos.c \
          src/ranking.c \
          src/screens/screen.c \
          src/screens/menu.c \
          src/screens/about.c \
          src/screens/settings.c \
          src/screens/game.c \
          src/screens/gameover.c
OBJ_GUI = $(SRC_GUI:.c=.o)
TARGET_GUI = caranguejo_gui
GUI_CFLAGS = -Wall -Wextra -std=c99 -Isrc -Isrc/screens
GUI_LDFLAGS = -lraylib -lm -lpthread -ldl -lrt -lX11

all: terminal

terminal: $(TARGET_TERMINAL)

$(TARGET_TERMINAL): $(OBJ_TERMINAL)
	$(CC) $(CFLAGS) $(OBJ_TERMINAL) -o $(TARGET_TERMINAL)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

gui: $(TARGET_GUI)

$(TARGET_GUI): $(SRC_GUI)
	$(CC) $(GUI_CFLAGS) $(SRC_GUI) -o $(TARGET_GUI) $(GUI_LDFLAGS)

clean:
	rm -f $(OBJ_TERMINAL) $(TARGET_TERMINAL) $(OBJ_GUI) $(TARGET_GUI)

.PHONY: all terminal gui clean
