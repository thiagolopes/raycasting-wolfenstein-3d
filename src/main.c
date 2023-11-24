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
#include "dda.c"
#include "map.h"
#include "algorithm.h"

#define MOUSE_VELOCITY 0.002909
bool SHADOWS = true;

typedef struct {
    double angle, direction_x, direction_y;
    float  x, y;
    int    pitch_view, fov;
} Player_t;

typedef struct {
    Point2h pos; // start position
    Point2h dir; // initial direction vector
    Point2h plane; // the 2d raycaster version of camera plane
} Player;
Player PLAYER = {{22.0, 12}, {-1.0, 0.0}, {0.0, 0.66}};

//x-coordinate in camera space
double camera_position(size_t x, size_t width){
    double position = 2 * x / (double)width - 1;
    return position;
}

typedef struct {
    int      run_status;
    Player_t Player;

    /* move to map */
    int map_cols;
    int map_rows;
    int map_height;
    int fps;
    int map_tile[24][24];
    /* engine */
} AppGame;

void update_player(Keys* keys, Player_t* player, Grid grid, float deltatime) {
    double move_speed = ONE_RAD / 4 * deltatime;
    double rot_speed = ONE_RAD / 4 * deltatime;

    if (keys->d == 1) {
        double old_dir_x = PLAYER.dir.x;
        PLAYER.dir.x = PLAYER.dir.x * cos(-rot_speed) - PLAYER.dir.y * sin(-rot_speed);
        PLAYER.dir.y = old_dir_x * sin(-rot_speed) + PLAYER.dir.y * cos(-rot_speed);

        double old_plane_x = PLAYER.plane.x;
        PLAYER.plane.x = PLAYER.plane.x * cos(-rot_speed) - PLAYER.plane.y * sin(-rot_speed);
        PLAYER.plane.y = old_plane_x * sin(-rot_speed) + PLAYER.plane.y * cos(-rot_speed);
    }
    if (keys->a == 1) {
        double old_dir_x = PLAYER.dir.x;
        //TODO this math is a matrix rotation; move to math
        PLAYER.dir.x = PLAYER.dir.x * cos(rot_speed) - PLAYER.dir.y * sin(rot_speed);
        PLAYER.dir.y = old_dir_x * sin(rot_speed) + PLAYER.dir.y * cos(rot_speed);

        double old_plane_x = PLAYER.plane.x;
        PLAYER.plane.x = PLAYER.plane.x * cos(rot_speed) - PLAYER.plane.y * sin(rot_speed);
        PLAYER.plane.y = old_plane_x * sin(rot_speed) + PLAYER.plane.y * cos(rot_speed);
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

static void handle_key(SDL_Keysym keysym, AppGame* App, Keys* keys, int button_action) {
    switch (keysym.sym) {
        case SDLK_ESCAPE:
            App->run_status = 0;
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

void handle_mouse_pressed_up(int button, float x, float y, AppGame* App) {
    switch (button) {
        case SDL_BUTTON_LEFT:
            break;
        case SDL_BUTTON_RIGHT:
            break;
    }
}

void handle_mouse_pressed_down(int button, float x, float y, AppGame* App) {
    switch (button) {
        case SDL_BUTTON_LEFT:
            break;
        case SDL_BUTTON_RIGHT:
            break;
    }
}


#define w 1920
#define h 1080
void draw_ray(double ray_dist, int side, Point2h ray_dir, Cel cel, int x_pos) {
    // Calculate height of line to draw on screen
    int wall_height = (int)(h / ray_dist);
    // calculate where the line start and ends, talken h as center of hall;
    int draw_start = -wall_height / 2 + h / 2;
    int draw_end   = wall_height / 2 + h / 2;

    double text_x;
    if (side == NS)
        text_x = 1 - (PLAYER.pos.y + ray_dist * ray_dir.y);
    else
        text_x = 1 - (PLAYER.pos.x + ray_dist * ray_dir.x);
    text_x -= floor(text_x);

    double color = height_shadow(ray_dist);
    draw_line_vertical(x_pos, draw_start, draw_end, cel.raw_value, text_x, (Color){color, color, color});
};

void new_3d_render(AppGame* app, Grid* map){
    Player r = PLAYER;

    for(size_t x = 0; x < w; x++){
        double camera_pos = camera_position(x, w);
        //calculate ray position and direction
        Point2h ray_dir = {r.dir.x + r.plane.x * camera_pos, r.dir.y + r.plane.y * camera_pos};
        //lenght of the ray while steps;
        double ray_dist;
        // raycaster
        Ray ray = ray_setup(PLAYER.pos, ray_dir);
        // was there a wall hit?
        bool hit = false;
        while(!hit){
            ray_next_step(&ray);
            if (grid_index_valid(*map, ray.map_grid.x, ray.map_grid.y)){
                if (map->cels[ray.map_grid.x][ray.map_grid.y].raw_value != 0) {
                    hit = true;
                }
            }
        }

        // camera plane dist
        if(ray.side == NS){
            ray_dist = (ray.side_dist.x - ray.delta_dist.x);
        }
        else{
            ray_dist = (ray.side_dist.y - ray.delta_dist.y);
        }
        // move to draw_ray
        draw_ray(ray_dist, ray.side, ray_dir, map->cels[ray.map_grid.x][ray.map_grid.y], x);
    }
}

void draw_3d_view_floor(AppGame* App, Window* win) {
    float      floor_start = win->height / 2;
    Rectanglef floor       = {0, floor_start, (float)win->width, (float)win->height + App->Player.pitch_view};
    draw_rectf_gradient(floor, BLACK, DARKGRAY);
}

void handle_mouse_motion(AppGame* App, SDL_Event* event) {
    if (event->motion.state == (SDL_BUTTON_LMASK | SDL_BUTTON_RMASK)) {
        handle_mouse_pressed_down(SDL_BUTTON_LEFT, event->button.x, event->button.y, App);
        handle_mouse_pressed_down(SDL_BUTTON_RIGHT, event->button.x, event->button.y, App);
    } else if (event->motion.state == SDL_BUTTON_LMASK) {
        handle_mouse_pressed_down(SDL_BUTTON_LEFT, event->button.x, event->button.y, App);
    } else if (event->motion.state == SDL_BUTTON_RMASK) {
        handle_mouse_pressed_down(SDL_BUTTON_RIGHT, event->button.x, event->button.y, App);
    }

    switch (event->type) {
        case SDL_MOUSEMOTION:
            break;
    }
}

// main
int main(int argc, char* args[]) {
    srand(time(NULL));
    char    title_format[] = "Simple Wolfenstein Engine - FPS %i";
    char    title[256]     = "Simple Wolfenstein Engine";
    AppGame App            = {1, {0, cos(TAU), -sin(TAU), 50, 50, 0, 70}, 24, 24, 32, 144, 0};

    /* Window window_deamon = window_wake_up(title, 2560, 1440, false); */
    Window window_deamon = window_wake_up(title, 1920, 1080, false);
    Keys   keys_map      = {0};
    Mouse  mouse         = {0};
    window_vsync(false);
    window_capture_cursor(true);

    Texture  t1          = texture_new("textures/doom/1.png", false);
    Texture  t2          = texture_new("textures/doom/2.png", false);
    Texture  t3          = texture_new("textures/doom/3.png", false);
    Texture  t4          = texture_new("textures/doom/4.png", false);
    Texture  t5          = texture_new("textures/doom/5.png", false);
    Texture  t6          = texture_new("textures/doom/6.png", false);
    Texture  t7          = texture_new("textures/doom/7.png", false);
    Texture  t8          = texture_new("textures/doom/8.png", false);
    Texture  t9          = texture_new("textures/test.png", false);
    Texture* textures[9] = {&t1, &t2, &t3, &t4, &t5, &t6, &t7, &t8, &t9};
    // TODO tab enable select texture

    Grid grid = grid_load("maps/map_01");
    for (int x = 0; x < App.map_cols; x++) {
        for (int y = 0; y < App.map_rows; y++) {
            App.map_tile[x][y] = grid.cels[x][y].raw_value;
        }
    }
    const double freq_ms       = SDL_GetPerformanceFrequency();
    Uint64       last_time     = SDL_GetPerformanceCounter();
    unsigned int frame_counter = 0;
    double       frame_timer   = last_time;
    const double frame_delta   = 1000.0 / App.fps;

    while (App.run_status) {
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
                    App.run_status = 0;
                    break;
                case SDL_KEYDOWN:
                    handle_key(event.key.keysym, &App, &keys_map, 1);
                    break;
                case SDL_KEYUP:
                    handle_key(event.key.keysym, &App, &keys_map, 0);
                    break;
                case SDL_MOUSEMOTION:
                    handle_mouse_motion(&App, &event);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    handle_mouse_pressed_down(event.button.button, event.button.x, event.button.y, &App);
                    break;
                case SDL_MOUSEBUTTONUP:
                    handle_mouse_pressed_up(event.button.button, event.button.x, event.button.y, &App);
                    break;
            }
        }

            /* update here */
            update_player(&keys_map, &App.Player, grid, delta);

            /* draw here */
            draw_3d_view_floor(&App, &window_deamon);
            new_3d_render(&App, &grid);

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
