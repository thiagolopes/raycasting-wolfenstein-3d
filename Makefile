CC = gcc
LIBS = -lGL -lGLU -lglut -lm

all:
	$(CC) main.c -o main $(LIBS)
