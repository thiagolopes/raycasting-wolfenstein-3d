#pragma once
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include "math.h"

#define TEXTURE_NO_OFFSET

typedef struct Window Window;
struct Window {
    SDL_Window* sdl_window_id;
    SDL_GLContext sdl_gl_context_id;
    char* name;
    int width;
    int height;
    bool fullscreen;
};

typedef struct Renderer Renderer;
struct Renderer {
};

typedef struct Keys Keys;
struct Keys {
    int a, w, s, d, shift, ctrl, j, k;
};

typedef struct Mouse Mouse;
struct Mouse {
    float x, y, xref, yref, button_r, button_l;
};

typedef struct Color Color;
struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

typedef struct Rectanglef Rectanglef;
struct Rectanglef {
    float x;
    float y;
    float width;
    float height;
};

Window window_wake_up(char* name, int height, int width, bool fullscreen);
void window_vsync(bool flag);
void window_fullscreen(Window* window, bool flag);
void window_capture_cursor(bool flag);
void window_start_frame();
void window_finish_frame(Window *window);
void window_shutdown(Window *window);

/* primitive draws calls */
void draw_rectf(Rectanglef rect, Color color);
void draw_rectf_gradient(Rectanglef r, Color c_top, Color c_bottom);
void draw_line(Point2f p1, Point2f p2, float texture_offset, Color color);
void draw_line_vertical(float posX, float posY, float posY_end, int texture_id, float texture_offset, Color color);
