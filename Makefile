##
## EPITECH PROJECT, 2025
## add
## File description:
## Makefile
##

CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -fPIC -O2 -I./src/interfaces
LDFLAGS = -shared -ldl

CORE_DIR = src/core
LIB_DIR = lib

CORE_SRCS = $(CORE_DIR)/main.cpp $(CORE_DIR)/Core.cpp $(CORE_DIR)/DLLoader.cpp
CORE_OBJS = $(CORE_SRCS:.cpp=.o)
CORE_EXEC = arcade

all: core graphicals games

core: $(CORE_EXEC)

$(CORE_EXEC):
	$(CXX) $(CXXFLAGS) $(CORE_SRCS) -o ./arcade

graphicals: $(LIB_DIR)/arcade_ncurses.so $(LIB_DIR)/arcade_sdl2.so $(LIB_DIR)/arcade_sfml.so

$(LIB_DIR)/arcade_ncurses.so:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LIB_DIR)/graphicals/Ncurses/Ncurses.cpp -o $(LIB_DIR)/arcade_ncurses.so -lncurses

$(LIB_DIR)/arcade_sdl2.so:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LIB_DIR)/graphicals/SDL2/SDL2.cpp -o $(LIB_DIR)/arcade_sdl2.so $(shell sdl2-config --cflags --libs) -lSDL2_ttf

$(LIB_DIR)/arcade_sfml.so:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LIB_DIR)/graphicals/SFML/SFML.cpp -o $(LIB_DIR)/arcade_sfml.so -lsfml-graphics -lsfml-window -lsfml-system

games: $(LIB_DIR)/arcade_pacman.so $(LIB_DIR)/arcade_nibbler.so $(LIB_DIR)/arcade_snake.so

$(LIB_DIR)/arcade_pacman.so:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LIB_DIR)/games/Pacman/Pacman.cpp -o $(LIB_DIR)/arcade_pacman.so

$(LIB_DIR)/arcade_nibbler.so:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LIB_DIR)/games/Nibbler/Nibbler.cpp -o $(LIB_DIR)/arcade_nibbler.so

$(LIB_DIR)/arcade_snake.so:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LIB_DIR)/games/Snake/Snake.cpp -o $(LIB_DIR)/arcade_snake.so

clean:
	rm -f $(CORE_OBJS)
	rm -f $(CORE_EXEC)
	rm -f $(LIB_DIR)/arcade_*.so

fclean: clean

re: fclean all

install:
	mkdir -p assets/maps/nibbler
	mkdir -p assets/maps/pacman
	mkdir -p assets/maps/snake
	cp -r assets/* $(DESTDIR)/assets/
