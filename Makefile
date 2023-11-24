CC=gcc
CCFLAGS=-Wextra -Wall -Wfloat-equal -Wshadow -ffunction-sections -g
LIBS=-lm -lSDL2 -lOpenGL

MAIN=src/main.c
FILES=src/math.c src/engine.c src/texture.c src/map.c src/algorithm.c

all: main run

main:
	 $(CC) $(MAIN) $(FILES) -o raycasting $(LIBS) $(CCFLAGS)

run:
	./raycasting

TAGS:
	ctags -e -R .
