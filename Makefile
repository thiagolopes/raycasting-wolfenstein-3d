CC=gcc
CFLAGS=-Wextra -Wall -Wfloat-equal -Wshadow -O2
LIBS=-lm -lSDL2 -lOpenGL

MAIN=src/main.c
FILES=src/math.c src/engine.c

main:
	 $(CC) $(MAIN) $(FILES) -o raycasting $(LIBS) $(CFLAGS)

run:
	./raycasting
