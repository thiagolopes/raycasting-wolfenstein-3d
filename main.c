#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <math.h>

#include <stdio.h>
#include <stdlib.h>

static void quit_tutorial(int code) {
  SDL_Quit();
  exit(code);
}

static void handle_key_down(SDL_keysym *keysym) {
  switch (keysym->sym) {
  case SDLK_ESCAPE:
    quit_tutorial(0);
    break;
  default:
    break;
  }
}

static void process_events(void) {
  /* Our SDL event placeholder. */
  SDL_Event event;

  /* Grab all the events off the queue. */
  while (SDL_PollEvent(&event)) {

    switch (event.type) {
    case SDL_KEYDOWN:
      /* Handle key presses. */
      handle_key_down(&event.key.keysym);
      break;
    case SDL_QUIT:
      /* Handle quit requests (like Ctrl-c). */
      quit_tutorial(0);
      break;
    }
  }
}

static void setup_opengl(int width, int height) {
  float ratio = (float)width / (float)height;

  /* Our shading model--Gouraud (smooth). */
  glShadeModel(GL_SMOOTH);

  /* Culling. */
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
  glEnable(GL_CULL_FACE);

  /* Set the clear color. */
  glClearColor(0, 0, 0, 0);

  /* Setup our viewport. */
  glViewport(0, 0, width, height);

  /*
   * Change to the projection matrix and set
   * our viewing volume.
   */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  /*
   * EXERCISE:
   * Replace this with a call to glFrustum.
   */
  gluPerspective(60.0, ratio, 1.0, 1024.0);
}

int main(int argc, char *argv[]) {
  const SDL_VideoInfo *info = NULL;
  int width = 1024;
  int height = 512;
  /* Color depth in bits of our window. */
  int bpp = 0;
  /* Flags we will pass into SDL_SetVideoMode. */
  int flags = 0;

  /* First, initialize SDL's video subsystem. */
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    /* Failed, exit. */
    fprintf(stderr, "Video initialization failed: %s\n", SDL_GetError());
    quit_tutorial(1);
  }

  /* Let's get some video information. */
  info = SDL_GetVideoInfo();
  if (!info) {
    /* This should probably never happen. */
    fprintf(stderr, "Video query failed: %s\n", SDL_GetError());
    quit_tutorial(1);
  }
  bpp = info->vfmt->BitsPerPixel;

  /*
   * Now, we want to setup our requested
   * window attributes for our OpenGL window.
   * We want *at least* 5 bits of red, green
   * and blue. We also want at least a 16-bit
   * depth buffer.
   *
   * The last thing we do is request a double
   * buffered window. '1' turns on double
   * buffering, '0' turns it off.
   *
   * Note that we do not use SDL_DOUBLEBUF in
   * the flags to SDL_SetVideoMode. That does
   * not affect the GL attribute state, only
   * the standard 2D blitting setup.
   */
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  flags = SDL_OPENGL;
  if (SDL_SetVideoMode(width, height, bpp, flags) == 0) {
    fprintf(stderr, "Video mode set failed: %s\n", SDL_GetError());
    quit_tutorial(1);
  }

  /*
   * At this point, we should have a properly setup
   * double-buffered window for use with OpenGL.
   */
  setup_opengl(width, height);

  while (1) {
    /* Process incoming events. */
    process_events();
    /* Draw the screen. */
  }

  /*
   * Record timings using SDL_GetTicks() and
   * and print out frames per second at program
   * end.
   */

  /* Never reached. */
  return 0;
}
