CC = gcc
LIBS = -lGL -lm -lSDL2 -lGLEW -lSDL2_image

all:
	$(CC) main.c -g -o main $(LIBS)
