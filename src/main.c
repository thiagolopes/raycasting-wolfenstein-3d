#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_opengl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <dirent.h>

#include <math.h>

// internal
#include "engine.h"
#include "texture.h"
#include "colors.h"
#include "map.h"
#include "algorithm.h"

#define MOUSE_VELOCITY 0.002909
bool SHADOWS;
int FPS;
bool RUN;

typedef struct {
    // start position
    Point2h pos;
    // initial direction vector
    Point2h dir;
    // the 2d raycaster version of camera plane
    Point2h plane;
    // total render width (px)
    size_t width;
    // total render height (px)
    size_t height;
} Camera;

Camera PLAYER = {{22.0, 12}, {-1.0, 0.0}, {0.0, 0.66}};

Point2h camera_get_plane_dir(Camera camera, size_t x) {
    // x coordinate in camera space (a vector)
    double position = 2 * x / (double)camera.width - 1;
    // calculate ray position and direction
    Point2h ray_dir = {camera.dir.x + camera.plane.x * position, camera.dir.y + camera.plane.y * position};
    return ray_dir;
}

void update_player(Keys* keys, Grid grid, float deltatime) {
    double move_speed = ONE_RAD / 4 * deltatime;
    double rot_speed  = ONE_RAD / 4 * deltatime;

    if (keys->d == 1) {
        PLAYER.pos.x += PLAYER.dir.y * move_speed;
        PLAYER.pos.y -= PLAYER.dir.x * move_speed;
    }
    if (keys->a == 1) {
        PLAYER.pos.x -= PLAYER.dir.y * move_speed;
        PLAYER.pos.y += PLAYER.dir.x * move_speed;
    }
    if (keys->w == 1) {
        PLAYER.pos.x += PLAYER.dir.x * move_speed;
        PLAYER.pos.y += PLAYER.dir.y * move_speed;
    }
    if (keys->s == 1) {
        PLAYER.pos.x -= PLAYER.dir.x * move_speed;
        PLAYER.pos.y -= PLAYER.dir.y * move_speed;
    }
    if (keys->j == 1) {
    }
    if (keys->k == 1) {
        SHADOWS = !SHADOWS;
    }
}

static void handle_key(SDL_Keysym keysym, Keys* keys, int button_action) {
    switch (keysym.sym) {
        case SDLK_ESCAPE:
            RUN = false;
            break;
        case SDLK_a:
            keys->a = button_action;
            break;
        case SDLK_d:
            keys->d = button_action;
            break;
        case SDLK_w:
            keys->w = button_action;
            break;
        case SDLK_s:
            keys->s = button_action;
            break;
        case SDLK_LSHIFT:
            keys->shift = button_action;
            break;
        case SDLK_j:
            keys->j = button_action;
            break;
        case SDLK_k:
            keys->k = button_action;
            break;
        case SDLK_LCTRL:
            keys->ctrl = button_action;
            break;
    }
}

void draw_aim(Window* window) {}

void handle_mouse_pressed_up(int button, float x, float y) {
    switch (button) {
        case SDL_BUTTON_LEFT:
            break;
        case SDL_BUTTON_RIGHT:
            break;
    }
}

void handle_mouse_pressed_down(int button, float x, float y) {
    switch (button) {
        case SDL_BUTTON_LEFT:
            break;
        case SDL_BUTTON_RIGHT:
            break;
    }
}

void draw_ray(double ray_dist, int side, Point2h ray_dir, Cel cel, int x_pos, int height) {
    int h = height;
    // Calculate height of line to draw on screen
    int wall_height = (int)(h / ray_dist);
    // calculate where the line start and ends, talken h as center of hall;
    int draw_start = -wall_height / 2 + h / 2;
    int draw_end   = wall_height / 2 + h / 2;

    double text_x;
    if (side == NS) {
        text_x = 1 - (PLAYER.pos.y + ray_dist * ray_dir.y);
    } else {
        text_x = 1 - (PLAYER.pos.x + ray_dist * ray_dir.x);
    }
    text_x -= floor(text_x);

    double color = 255;
    if (SHADOWS){
        color = height_shadow(ray_dist);
    }
    draw_line_vertical(x_pos, draw_start, draw_end, cel.raw_value, text_x, (Color){color, color, color});
};

void render_fps(int h, Grid* map) {
    Camera r = PLAYER;

    // cast a ray for every camera position
    for (size_t x = 0; x < r.width; x++) {
        Point2h ray_dir = camera_get_plane_dir(r, x);
        // raycaster
        Ray ray = ray_setup(PLAYER.pos, ray_dir);
        // was there a wall hit?
        bool hit = false;
        while (!hit) {
            ray_next_step(&ray);
            if (grid_index_valid(*map, ray.map_grid.x, ray.map_grid.y)) {
                if (map->cels[ray.map_grid.x][ray.map_grid.y].raw_value != 0) {
                    hit = true;
                }
            }
        }
        // camera plane dist
        double ray_dist = ray_get_dist(&ray);

        // move to draw_ray
        draw_ray(ray_dist, ray.side, ray_dir, map->cels[ray.map_grid.x][ray.map_grid.y], x, h);
    }
}

void handle_mouse_motion(SDL_Event* event) {
    if (event->motion.state == (SDL_BUTTON_LMASK | SDL_BUTTON_RMASK)) {
        handle_mouse_pressed_down(SDL_BUTTON_LEFT, event->button.x, event->button.y);
        handle_mouse_pressed_down(SDL_BUTTON_RIGHT, event->button.x, event->button.y);
    } else if (event->motion.state == SDL_BUTTON_LMASK) {
        handle_mouse_pressed_down(SDL_BUTTON_LEFT, event->button.x, event->button.y);
    } else if (event->motion.state == SDL_BUTTON_RMASK) {
        handle_mouse_pressed_down(SDL_BUTTON_RIGHT, event->button.x, event->button.y);
    }

    switch (event->type) {
        case SDL_MOUSEMOTION:
            PLAYER.dir   = point2h_rotate(PLAYER.dir, -event->motion.xrel * MOUSE_VELOCITY);
            PLAYER.plane = point2h_rotate(PLAYER.plane, -event->motion.xrel * MOUSE_VELOCITY);
            break;
    }
}

// main
int main(int argc, char* args[]) {
    srand(time(NULL));
    char title_format[] = "Simple Wolfenstein Engine - FPS %i";
    char title[256]     = "Simple Wolfenstein Engine";

    SHADOWS = true;
    FPS = 144;
    RUN = true;

    Window window_deamon = window_wake_up(title, 2560, 1440, false);
    /* Window window_deamon = window_wake_up(title, 1920, 1080, false); */

    Keys  keys_map = {0};
    Mouse mouse    = {0};
    window_vsync(false);
    window_capture_cursor(true);

    PLAYER.width = window_deamon.width;
    PLAYER.height = window_deamon.height;

    Texture t1 = texture_new("textures/doom/1.png", false);
    Texture t2 = texture_new("textures/doom/2.png", false);
    Texture t3 = texture_new("textures/doom/3.png", false);
    Texture t4 = texture_new("textures/doom/4.png", false);
    Texture t5 = texture_new("textures/doom/5.png", false);
    Texture t6 = texture_new("textures/doom/6.png", false);
    Texture t7 = texture_new("textures/doom/7.png", false);
    Texture t8 = texture_new("textures/doom/8.png", false);
    Texture t9 = texture_new("textures/test.png", false);

    // TODO tab enable select texture
    Texture* textures[9] = {&t1, &t2, &t3, &t4, &t5, &t6, &t7, &t8, &t9};

    Grid grid = grid_load("maps/map_01");

    const double freq_ms       = SDL_GetPerformanceFrequency();
    Uint64       last_time     = SDL_GetPerformanceCounter();
    unsigned int frame_counter = 0;
    double       frame_timer   = last_time;
    const double frame_delta   = 1000.0 / FPS;

    while (RUN) {
        window_start_frame();
        Uint64 current_time = SDL_GetPerformanceCounter();
        double delta        = (current_time - last_time) / freq_ms * 1000.0;
        if (current_time > frame_timer + freq_ms) {
            sprintf(title, title_format, frame_counter);
            SDL_SetWindowTitle(window_deamon.sdl_window_id, title);
            frame_counter = 0;
            frame_timer   = current_time;
        }

        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            switch (event.type) {
                case SDL_QUIT:
                    RUN = false;
                    break;
                case SDL_KEYDOWN:
                    handle_key(event.key.keysym, &keys_map, 1);
                    break;
                case SDL_KEYUP:
                    handle_key(event.key.keysym, &keys_map, 0);
                    break;
                case SDL_MOUSEMOTION:
                    handle_mouse_motion(&event);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    handle_mouse_pressed_down(event.button.button, event.button.x, event.button.y);
                    break;
                case SDL_MOUSEBUTTONUP:
                    handle_mouse_pressed_up(event.button.button, event.button.x, event.button.y);
                    break;
            }
        }

        /* update here */
        update_player(&keys_map, grid, delta);

        /* draw here */
        render_fps(window_deamon.height, &grid);
        {
            texture_bind(t9);
            draw_rectf((Rectanglef){30, 30, 200, 200}, WHITE);
            texture_unbind();
        }
        window_finish_frame(&window_deamon);

        last_time = current_time;
        ++frame_counter;

        if (delta > frame_delta) {
            // here in capped by fps
            /* last_time = current_time; */
            /* ++frame_counter; */
        }
    }

    /* shutdown */
    window_shutdown(&window_deamon);
    return 0;
}
