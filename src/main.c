#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_opengl.h>
#include <stdbool.h>
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
/* #include "algorithm.h" */

// imgui
// #include "external/imgui/backends/imgui_impl_opengl3.h"
// #include "external/imgui/backends/imgui_impl_sdl2.h"
// #include "external/imgui/imgui.h"

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
    double moveSpeed = ONE_RAD / 4 * deltatime;
    double rotSpeed = ONE_RAD / 4 * deltatime;

    /* update angle to use a const * delta frame */

    /* FIX TO USE THE RELATIVE X/Y REF BASED ON THE PLAYER VIEW */
    if (keys->d == 1) { // right
        double old_dir_x = PLAYER.dir.x;
        PLAYER.dir.x = PLAYER.dir.x * cos(-rotSpeed) - PLAYER.dir.y * sin(-rotSpeed);
        PLAYER.dir.y = old_dir_x * sin(-rotSpeed) + PLAYER.dir.y * cos(-rotSpeed);

        double old_plane_x = PLAYER.plane.x;
        PLAYER.plane.x = PLAYER.plane.x * cos(-rotSpeed) - PLAYER.plane.y * sin(-rotSpeed);
        PLAYER.plane.y = old_plane_x * sin(-rotSpeed) + PLAYER.plane.y * cos(-rotSpeed);
    }
    if (keys->a == 1) {
        double old_dir_x = PLAYER.dir.x;
        //TODO this math is a matrix rotation; move to math
        PLAYER.dir.x = PLAYER.dir.x * cos(rotSpeed) - PLAYER.dir.y * sin(rotSpeed);
        PLAYER.dir.y = old_dir_x * sin(rotSpeed) + PLAYER.dir.y * cos(rotSpeed);

        double old_plane_x = PLAYER.plane.x;
        PLAYER.plane.x = PLAYER.plane.x * cos(rotSpeed) - PLAYER.plane.y * sin(rotSpeed);
        PLAYER.plane.y = old_plane_x * sin(rotSpeed) + PLAYER.plane.y * cos(rotSpeed);
    }
    if (keys->w == 1) {
        PLAYER.pos.x += PLAYER.dir.x * moveSpeed;
        PLAYER.pos.y += PLAYER.dir.y * moveSpeed;
    }
    if (keys->s == 1) {
        PLAYER.pos.x -= PLAYER.dir.x * moveSpeed;
        PLAYER.pos.y -= PLAYER.dir.y * moveSpeed;
    }
    if (keys->j == 1) {
    }
    if (keys->k == 1) {
        SHADOWS = !SHADOWS;
    }
/* #define H 32 */
/* #define BODY_AREA 4 */
/*     int o_mx = (int)(player->x / H); */
/*     int o_my = (int)(player->y / H); */
/*     int mx   = (int)((player->x + (dx * BODY_AREA)) / H); */
/*     int my   = (int)((player->y + (dy * BODY_AREA)) / H); */

/*     if (grid.cels[o_my][mx].raw_value == 0) { */
/*         player->x = new_px; */
/*     } */
/*     if (grid.cels[my][o_mx].raw_value == 0) { */
/*         player->y = new_py; */
/*     } */
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


typedef struct {
    //length of ray from current position to next x or y-side
    Point2h side_dist;
    //what direction to step in x or y-direction (either +1 or -1)
    Point2i step;
} Ray_step;

//calculate step and initial side
Ray_step ray_create_step(Point2h pos, Point2h dir, Point2i grid, Point2h delta_dist){
    Point2h side_dist;
    Point2i step;

    // set steps base on direction growth on the grid;
    if (dir.x < 0) {
            step.x = -1;
            side_dist.x = (pos.x - grid.x) * delta_dist.x;
    }
    else {
            step.x = 1;
            side_dist.x = (grid.x + 1.0 - pos.x) * delta_dist.x;
    }
    // is left
    if (dir.y < 0) {
        step.y = -1;
        side_dist.y = (pos.y - grid.y) * delta_dist.y;
    }
    // is right
    else {
        step.y = 1;
        side_dist.y = (grid.y + 1.0 - pos.y) * delta_dist.y;
    }

    Ray_step ray_step = {side_dist, step};
    return ray_step;
}

// walk ray to the next x and y map grid;
void ray_next_step_on(Point2h delta_dist, Point2i step, Point2h* side_dist, Point2i* map_grid, int* side) {
    // jump to next map square, either in x-direction, or in y-direction
    if (side_dist->x < side_dist->y) {
        side_dist->x += delta_dist.x;
        map_grid->x += step.x;
        *side = 0;
    } else {
        side_dist->y += delta_dist.y;
        map_grid->y += step.y;
        *side = 1;
    }
};


void draw_ray(){

};

void new_3d_render(AppGame* app, Grid* map){
    Player r = PLAYER;
    #define w 1920
    #define h 1080


    for(size_t x = 0; x < w; x++){
        //x-coordinate in camera space
        double camera_pos = 2 * x / (double)w - 1;
        //calculate ray position and direction
        Point2h ray_dir = {r.dir.x + r.plane.x * camera_pos, r.dir.y + r.plane.y * camera_pos};
        //which box of the map ray are in
        Point2i map_grid = {(int)r.pos.x, (int)r.pos.y};
        //length of ray from one x or y-side to next x or y-side
        Point2h delta_dist = {fabs(1 / ray_dir.x), fabs(1 / ray_dir.y)};
        //lenght of the ray while steps;
        double ray_dist;

        bool hit = false; //was there a wall hit?
        int side; //was a NS or a EW wall hit?

        Ray_step s = ray_create_step(PLAYER.pos, ray_dir, map_grid, delta_dist);
        Point2i step = s.step;
        Point2h side_dist = s.side_dist;

        if (ray_dir.x < 0) {
            step.x = -1;
            side_dist.x = (r.pos.x - map_grid.x) * delta_dist.x;
        } else {
            step.x = 1;
            side_dist.x = (map_grid.x + 1.0 - r.pos.x) * delta_dist.x;
        }
        if (ray_dir.y < 0) {
            step.y = -1;
            side_dist.y = (r.pos.y - map_grid.y) * delta_dist.y;
        } else {
            step.y = 1;
            side_dist.y = (map_grid.y + 1.0 - r.pos.y) * delta_dist.y;
        }

        while(!hit){
            ray_next_step_on(delta_dist, step, &side_dist, &map_grid, &side);
            if (grid_index_valid(*map, map_grid.x, map_grid.y)){
                if (map->cels[map_grid.x][map_grid.y].raw_value != 0) {
                    hit = true;
                }
            }
        }

        // camera plane dist
        if(side == 0){
            ray_dist = (side_dist.x - delta_dist.x);
        }
        else{
            ray_dist = (side_dist.y - delta_dist.y);
        }
        // move to draw_ray
        //Calculate height of line to draw on screen
        int wall_height = (int)(h / ray_dist);
        // calculate where the line start and ends, talken h as center of hall;
        int draw_start = -wall_height / 2 + h / 2;
        int draw_end = wall_height / 2 + h / 2;
        /* if (draw_start < 0) */
            /* draw_start = 0; */
        /* if (draw_end >= h) */
            /* draw_end = h - 1; */

      double wall_x;
      if (side == 0)
          wall_x = PLAYER.pos.y + ray_dist * ray_dir.y;
      else
          wall_x = PLAYER.pos.x + ray_dist * ray_dir.x;
      wall_x -= floor((wall_x));

      #define SHADOW_MAX 25
      #define NEAR 0.1
      #define FAR 0.70
      Color   draw_color = {0};
      float dd = ray_dist / SHADOW_MAX;
      float cp = 1.0 - exp(-dd * dd / (FAR * NEAR));
      float color  = 255 - 255 * cp;
      draw_color.r = color;
      draw_color.g = color;
      draw_color.b = color;

      draw_line_vertical(x, draw_start, draw_end, map->cels[map_grid.x][map_grid.y].raw_value, wall_x, draw_color);
    }
}

void draw_3d_view_floor(AppGame* App, Window* win) {
    float      floor_start = win->height / 2;
    Rectanglef floor       = {0, floor_start, (float)win->width, (float)win->height + App->Player.pitch_view};
    draw_rectf_gradient(floor, BLACK, DARKGRAY);
}

void draw(AppGame* App, Window* win, Grid* grid) {
    draw_3d_view_floor(App, win);
    draw_aim(win);
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
            draw(&App, &window_deamon, &grid);
            new_3d_render(&App, &grid);

            {
                texture_bind(t9);
                draw_rectf((Rectanglef){30, 30, 200, 200}, WHITE);
                texture_unbind();
            }
            window_finish_frame(&window_deamon);

            last_time = current_time;
            ++frame_counter;
        /* if (delta > frame_delta) { */
        /* } */
    }

    /* shutdown */
    window_shutdown(&window_deamon);
    return 0;
}
