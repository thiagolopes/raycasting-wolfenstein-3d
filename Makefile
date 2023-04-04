CC = gcc
LIBS = -lGL -lGLU -lglut -lm -lSDL

all:
	$(CC) main.c -g -o main $(LIBS)
