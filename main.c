#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>

#define WIDTH 1024
#define HEIGHT 512

#define PI M_PI

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
  int x, y;
  float x_delta, y_delta, angle;
} Player;

Player player;


void drawPlayer() {
  glColor3f(1, 1, 1);
  glPointSize(8);
  glBegin(GL_POINTS);
  glVertex2i(player.x, player.y);
  glEnd();

  glLineWidth(3);
  glBegin(GL_LINES);
  glVertex2i(player.x, player.y);
  glVertex2i(player.x + player.x_delta * 5, player.y + player.y_delta * 5);
  glEnd();
}

void handlerKeyPressed(unsigned char key, int x, int y) {
  switch (key) {
  case 'a':
    player.angle -= .1f;
    if (player.angle < 0)
      player.angle += PI * 2;

    player.x_delta = cos(player.angle) * 5;
    player.y_delta = sin(player.angle) * 5;
    break;
  case 'd':
    player.angle += .1f;
    if (player.angle > PI * 2)
      player.angle -= PI * 2;

    player.x_delta = cos(player.angle) * 5;
    player.y_delta = sin(player.angle) * 5;
    break;
  case 'w':
    player.y += player.y_delta;
    player.x += player.x_delta;
    break;
  case 's':
    player.y -= player.y_delta;
    player.x -= player.x_delta;
    break;
  }
  glutPostRedisplay();
}

void drawDisplay() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* draw here */
  drawMap2D();
  drawPlayer();

  printf("x=%i, y=%i=, dx=%f, dy=%f, pa=%f\n", player.x, player.y, player.x_delta, player.y_delta, player.angle);

  glutSwapBuffers();
}

void init() {
  glClearColor(.3f, .3f, .3f, 0);
  gluOrtho2D(0, WIDTH, HEIGHT, 0);

  player.x = 300;
  player.y = 300;
  player.angle = 2 * PI;
  player.x_delta = cos(player.angle) * 5;
  player.y_delta = sin(player.angle) * 5;
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
