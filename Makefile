main:
	 gcc src/main.c src/math.c -o raycasting -lm -lSDL2 -lOpenGL

run:
	./raycasting
