CC=gcc
CFLAGS=-Wextra -Wall -Wfloat-equal -Wshadow -ffunction-sections
LIBS=-lm -lSDL2 -lOpenGL

MAIN=src/main.c
FILES=src/math.c src/engine.c src/texture.c

main:
	 $(CC) $(MAIN) $(FILES) -o raycasting $(LIBS) $(CFLAGS)

run:
	./raycasting
