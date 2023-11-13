#pragma once
#include <stdbool.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

typedef struct{
  SDL_Window* sdl_window_id;
  SDL_GLContext sdl_gl_context_id;
  char* name;
  int width;
  int height;
  bool fullscreen;
}Window;

typedef struct{
    int a, w, s, d, shift, ctrl, j, k;
} Keys;

typedef struct{
    float x, y, xref, yref, button_r, button_l;
} Mouse;

typedef struct{
    unsigned char r; // Color red value
    unsigned char g; // Color green value
    unsigned char b; // Color blue value
    unsigned char a; // Color alpha value
} Color;

Window window_wake_up(char* name, int height, int width, bool fullscreen);
void window_vsync(bool flag);
void window_fullscreen(Window* window, bool flag);
void window_keep_mouse_on_center(bool flag);
void window_finish_frame(Window *window);
void window_shutdown(Window *window);
