CC = gcc
LIBS = -lGL -lGLU -lglut

all:
	$(CC) main.c -o main $(LIBS)
