CC=gcc
CFLAGS=-I.
LIBS=-lm -lSDL2 -lOpenGL

main:
	 $(CC) src/main.c src/math.c src/engine.c -o raycasting $(LIBS) $(CFLAGS)

run:
	./raycasting
