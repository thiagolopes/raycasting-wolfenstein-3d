CC = gcc
LIBS = -lGL -lm -lSDL2 -lGLEW -lSDL2_image

all:
	$(CC) src/main.c -g -o main $(LIBS)

lint:
	@clang-format --style=file src/*.c -i
