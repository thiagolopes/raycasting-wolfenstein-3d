#include <GL/freeglut_std.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#define WIDTH 800
#define HEIGHT 600

void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glutSwapBuffers();
}


void init(){
  glClearColor(.3f, .3f, .3f, 0);
  gluOrtho2D(0, WIDTH, HEIGHT, 0);
}

int main(int argc, char *argv[]){
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize(WIDTH, HEIGHT);
  glutCreateWindow("");
  init();
  glutDisplayFunc(display);
  glutMainLoop();
}
