CC = gcc
LIBS = -lGL -lm -lSDL2

all:
	$(CC) main.c -g -o main $(LIBS)
