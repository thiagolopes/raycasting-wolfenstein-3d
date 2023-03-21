#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>

#define WIDTH 1024
#define HEIGHT 512

int mapX = 8, mapY = 8, mapS = 64;
int map[] = {
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 0, 1, 0, 0, 0, 0, 1,
  1, 0, 1, 0, 0, 0, 0, 1,
  1, 0, 1, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 1, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
};


void drawMap2D() {
  int xo, yo;
  for (int y = 0; y < mapY; y++) {
    for (int x = 0; x < mapX; x++) {
      if (map[y * mapX + x]) {
        glColor3f(1, 0, 1);
      } else {
        glColor3f(0, 0, 1);
      }

      int xo = x * mapS;
      int yo = y * mapS;

      glBegin(GL_QUADS);
      glVertex2i(xo + 1, yo + 1);
      glVertex2i(xo + 1, yo + mapS - 1);
      glVertex2i(xo + mapS - 1, yo + mapS - 1);
      glVertex2i(xo + mapS - 1, yo + 1);
      glEnd();
    }
  }
}

typedef struct Player {
  int x;
  int y;
} Player;

Player player = {300, 300};

void drawPlayer() {
  glColor3f(1, 1, 0);
  glPointSize(8);
  glBegin(GL_POINTS);
  glVertex2i(player.x, player.y);
  glEnd();
}

void handlerKeyPressed(unsigned char key, int x, int y) {
  switch (key) {
  case 'a':
    player.x -= 5;
    break;
  case 'd':
    player.x += 5;
    break;
  case 'w':
    player.y -= 5;
    break;
  case 's':
    player.y += 5;
    break;
  }
  glutPostRedisplay();
}

void drawDisplay() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* draw here */
  drawMap2D();
  drawPlayer();

  glutSwapBuffers();
}

void init() {
  glClearColor(.3f, .3f, .3f, 0);
  gluOrtho2D(0, WIDTH, HEIGHT, 0);
}

int main(int argc, char *argv[]) {
  /* TODO - comment all opengl stuff, like a documentation resume*/
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize(WIDTH, HEIGHT);
  glutCreateWindow("");
  init();
  glutDisplayFunc(drawDisplay);
  glutKeyboardFunc(handlerKeyPressed);
  glutMainLoop();
}
