#include "engine.h"
#include <GL/gl.h>

void window_finish_frame(Window *window){
  SDL_GL_SwapWindow(window->sdl_window_id);
}

void window_vsync(bool flag){
  if (SDL_GL_SetSwapInterval((int)flag) < 0) {
    perror(SDL_GetError());
  };
  printf("Vsync set to: %i\n", flag);
};

void window_fullscreen(Window* window, bool flag){
  SDL_SetWindowFullscreen(window->sdl_window_id, SDL_WINDOW_FULLSCREEN);
}

/* disable the cursor too by sanity */
void window_capture_cursor(bool flag){
  if (flag) {
    SDL_ShowCursor(SDL_DISABLE);
    SDL_CaptureMouse(SDL_TRUE);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
  } else {
    SDL_ShowCursor(SDL_ENABLE);
    SDL_CaptureMouse(SDL_FALSE);
    SDL_SetRelativeMouseMode(SDL_FALSE);
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "0");
  }
}

Window window_wake_up(char* name, int width, int height, bool fullscreen){
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVERYTHING) < 0) {
        perror(SDL_GetError());
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window* window;
    SDL_GLContext gl_context;
    window = SDL_CreateWindow(name,
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              width,
                              height,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);

    if (window == NULL) {
        perror(SDL_GetError());
    }

    gl_context = SDL_GL_CreateContext(window);
    if (gl_context == NULL) {
        perror(SDL_GetError());
    }

    /* start basic ORTHO projection TODO move to shader MVP or render*/
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, 1, -1);
    glMatrixMode(GL_MODELVIEW);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    Window window_deamon = {window, gl_context, name, width, height, fullscreen};
    return window_deamon;
}

void window_shutdown(Window *runtime){
    SDL_VideoQuit();
    SDL_GL_DeleteContext(runtime->sdl_gl_context_id);
    SDL_DestroyWindow(runtime->sdl_window_id);
    SDL_Quit();
}

void draw_rectf(Rectanglef rect, Color color){
    glColor4ub(color.r, color.g, color.b, color.a);
    glBegin(GL_QUADS);

    glTexCoord2d(0.0, 1.0);
    glVertex2i(rect.x, rect.y);

    glTexCoord2d(1.0, 1.0);
    glVertex2i(rect.x + rect.width, rect.y);

    glTexCoord2d(1.0, 0.0);
    glVertex2i(rect.x + rect.width, rect.y + rect.height);

    glTexCoord2d(0.0, 0.0);
    glVertex2i(rect.x, rect.y + rect.height);

    glEnd();
}

void draw_line(float posX, float posY, float posY_end, int texture_id, float texture_offset, Color color){
    glColor4ub(color.r, color.g, color.b, color.a);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glBegin(GL_LINES);

    glTexCoord2d(texture_offset, 1.0);
    glVertex2i(posX, posY);

    glTexCoord2d(texture_offset, 0.0);
    glVertex2i(posX, posY_end);

    glEnd();
    glDisable(GL_TEXTURE_2D); // TODO remove this after bind use in code
}
