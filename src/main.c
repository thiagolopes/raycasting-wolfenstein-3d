#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_opengl.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <dirent.h>
#include <GL/gl.h>

// internal
#include "engine.h"
#include "texture.h"
#include "colors.h"
#include "dda.c"
#include "map.h"

// imgui
// #include "external/imgui/backends/imgui_impl_opengl3.h"
// #include "external/imgui/backends/imgui_impl_sdl2.h"
// #include "external/imgui/imgui.h"

#define MOUSE_VELOCITY 0.002909

typedef struct {
    double angle, direction_x, direction_y;
    float  x, y;
    int    pitch_view, fov;
} Player_t;

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

int MAP[24][24] = {{8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 4, 6, 4, 4, 6, 4, 6, 4, 4, 4, 6, 4},
                   {8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
                   {8, 0, 9, 3, 0, 0, 0, 0, 0, 8, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6},
                   {8, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6},
                   {8, 0, 3, 3, 0, 0, 0, 0, 0, 8, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
                   {8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 4, 0, 0, 0, 0, 0, 6, 6, 6, 0, 6, 4, 6},
                   {8, 8, 8, 8, 0, 8, 8, 8, 8, 8, 8, 4, 4, 4, 4, 4, 4, 6, 0, 0, 0, 0, 0, 6},
                   {3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 8, 0, 8, 0, 8, 0, 8, 4, 0, 4, 0, 6, 0, 6},
                   {3, 7, 0, 0, 0, 0, 0, 0, 3, 8, 0, 8, 0, 8, 0, 8, 8, 6, 0, 0, 0, 0, 0, 6},
                   {6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 6, 0, 0, 0, 0, 0, 4},
                   {6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 6, 0, 6, 0, 6, 0, 6},
                   {3, 7, 0, 0, 0, 0, 0, 0, 3, 8, 0, 8, 0, 8, 0, 8, 8, 6, 4, 6, 0, 6, 6, 6},
                   {3, 3, 3, 3, 0, 3, 3, 3, 3, 8, 8, 4, 0, 6, 8, 4, 8, 3, 3, 3, 0, 3, 3, 3},
                   {2, 2, 2, 2, 0, 2, 2, 2, 2, 4, 6, 4, 0, 0, 6, 0, 6, 3, 0, 0, 0, 0, 0, 3},
                   {2, 2, 0, 0, 0, 0, 0, 2, 2, 4, 0, 0, 0, 0, 0, 0, 4, 3, 0, 0, 0, 0, 0, 3},
                   {2, 0, 0, 0, 0, 0, 0, 0, 2, 4, 0, 0, 0, 0, 0, 0, 4, 3, 0, 0, 0, 0, 0, 3},
                   {1, 0, 0, 0, 0, 0, 0, 0, 1, 4, 4, 4, 4, 4, 6, 0, 6, 3, 3, 0, 0, 0, 3, 3},
                   {2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 1, 2, 2, 2, 6, 6, 0, 0, 5, 0, 5, 0, 5},
                   {2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 2, 2, 0, 5, 0, 5, 0, 0, 0, 5, 5},
                   {2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 2, 5, 0, 5, 0, 5, 0, 5, 0, 5},
                   {1, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5},
                   {2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 2, 5, 0, 5, 0, 5, 0, 5, 0, 5},
                   {2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 2, 2, 0, 5, 0, 5, 0, 0, 0, 5, 5},
                   {2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 5, 5, 5, 5, 5, 5, 5, 5, 5}};

void update_player(Keys* keys, Player_t* player) {
    float magntude = 1;
    if (keys->shift == 1)
        magntude = 2;

    float dx = 0, dy = 0;
    float new_px=0, new_py=0;

    /* FIX TO USE THE RELATIVE X/Y REF BASED ON THE PLAYER VIEW */
    if (keys->d == 1) {
        dy -= player->direction_x * magntude;
        dx += player->direction_y * magntude;
    }
    if (keys->a == 1) {
        dy += player->direction_x * magntude;
        dx -= player->direction_y * magntude;
    }
    if (keys->w == 1) {
        dx += player->direction_x * magntude;
        dy += player->direction_y * magntude;
    }
    if (keys->s == 1) {
        dx -= player->direction_x * magntude;
        dy -= player->direction_y * magntude;
    }
    if (keys->j == 1) {
        player->fov -= 1;
    }
    if (keys->k == 1) {
        player->fov += 1;
    }

    new_px = player->x + dx;
    new_py = player->y + dy;

#define H 32
#define BODY_AREA 4
    int o_mx = (int)(player->x / H);
    int o_my = (int)(player->y / H);
    int mx = (int)((player->x + (dx * BODY_AREA)) / H);
    int my = (int)((player->y + (dy * BODY_AREA)) / H);

    if (MAP[o_my][mx] == 0){
        player->x = new_px;
    }
    if (MAP[my][o_mx] == 0){
        player->y = new_py;
    }

    player->angle = normalize_rand(player->angle);
}

void update(AppGame *App, Keys* keys) {
    update_player(keys, &App->Player);
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
        keys->k = 0;
        break;
    case SDLK_k:
        keys->k = button_action;
        keys->j = 0;
        break;
    case SDLK_LCTRL:
        keys->ctrl = button_action;
        break;
    }
}


void draw_aim(Window *window) {
}

void handle_mouse_pressed_up(int button, float x, float y, AppGame *App) {
    switch (button) {
    case SDL_BUTTON_LEFT:
        break;
    case SDL_BUTTON_RIGHT:
        break;
    }
}

void handle_mouse_pressed_down(int button, float x, float y, AppGame *App) {
    switch (button) {
    case SDL_BUTTON_LEFT:
        break;
    case SDL_BUTTON_RIGHT:
        break;
    }
}

void draw_3d_view_port(AppGame *App, Window *win, Texture* t) {
    int pixels_cols    = win->width;
    int draw_screen_h  = win->height;
    int wall_height    = App->map_height;
    int map_len        = App->map_cols;
    int(*map)[24]      = App->map_tile;
    int   pitch        = App->Player.pitch_view; /* player look up or down */
    float player_angle = App->Player.angle;
    float fov_in_rad   = degree_to_rad(App->Player.fov);
    float hfov_in_rad  = degree_to_rad(App->Player.fov / 2);
    float pixel_in_rad = fov_in_rad / pixels_cols;

    Point2f point_start = {App->Player.x, App->Player.y};
    Color   draw_color;
    draw_color.a             = 255;
    float fade_draw_vertical = 1.5;

    for (int pixel = 0; pixel < pixels_cols; pixel++) {
        int     line_h;
        int     line_start;
        int     line_end;
        float   texture_offset;
        float   angle;
        float   distance;
        Point2f point_end;
        DDA_t   ray_collision;
        float distance_len;
        Point2f distance_xy;

        angle       = normalize_rand(normalize_rand(player_angle - hfov_in_rad) + pixel_in_rad * pixel);
        point_end.x = point_start.x + cos(angle);
        point_end.y = point_start.y + -sin(angle);

        ray_collision = DDA(map, wall_height, map_len, point_end, point_start);

        if (ray_collision.valid) {
            distance_xy = point2f_sub(point_start, ray_collision.collision_point);
            distance_len = point2f_len(distance_xy);
            distance = fix_eye_fish(distance_xy, player_angle - angle);

            line_h     = (wall_height * draw_screen_h) / distance;
            line_start = (draw_screen_h / 2 - line_h / 2) - pitch;
            line_end   = line_start + line_h;

#define SHADOW_MAX 500.0
#define FAR 2.0
#define NEAR 0.1
            float dd = 1;
            if (distance_len < SHADOW_MAX){
                dd = distance_len / SHADOW_MAX;
            }
            float cp = (dd * (FAR - NEAR) + (FAR + NEAR)) / 2.0;
            float color = 255 - 255 * cp;
            draw_color.r = color;
            draw_color.g = color;
            draw_color.b = color;

            if (ray_collision.side == HORIZONTAL) {
                texture_offset = (float)((int)(ray_collision.collision_point.x) % wall_height) / wall_height;
            } else {
                texture_offset = (float)((int)(ray_collision.collision_point.y) % wall_height) / wall_height;
            }
            draw_line_vertical(pixel, line_start, line_end, ray_collision.grid_index_collision, texture_offset, draw_color);
        }
    }

}

void draw_3d_view_floor(AppGame* App, Window* win) {
    float floor_start = win->height / 2 - App->Player.pitch_view;
    Rectanglef floor = {0, floor_start, (float)win->width,
                        (float)win->height + App->Player.pitch_view};
    draw_rectf_gradient(floor, BLACK, DARKGRAY);
}

void draw(AppGame* App, Window* win, Texture* texture) {
    draw_3d_view_floor(App, win);
    draw_3d_view_port(App, win, texture);
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
        App->Player.pitch_view += (event->motion.yrel * 3);
        App->Player.angle += MOUSE_VELOCITY * event->motion.xrel;
        App->Player.direction_x = cos(App->Player.angle);
        App->Player.direction_y = -sin(App->Player.angle);
        break;
    }
}

// main
int main(int argc, char *args[]) {
    srand(time(NULL));
    char    title_format[] = "Simple Wolfenstein Engine - FPS %i";
    char    title[256]     = "Simple Wolfenstein Engine";
    AppGame App            = {1, {0, cos(TAU), -sin(TAU), 50, 50, 0, 70}, 24, 24, 32, 144, 0};
    for (int x = 0; x < App.map_cols; x++)
        for (int y = 0; y < App.map_rows; y++)
            App.map_tile[x][y] = MAP[x][y];

    /* Window window_deamon = window_wake_up(title, 2560, 1440, false); */
    Window window_deamon = window_wake_up(title, 1920, 1080, false);
    Keys keys_map = {0};
    Mouse mouse = {0};
    window_vsync(true);
    window_capture_cursor(true);

    Texture t1 = texture_new("textures/brick_white.png", false);
    Texture t2 = texture_new("textures/wall.png", false);
    Texture t3 = texture_new("textures/facility.png", false);
    Texture t4 = texture_new("textures/grass.png", false);
    Texture t5 = texture_new("textures/hack_1.png", false);
    Texture t6 = texture_new("textures/hack_2.png", false);
    Texture t7 = texture_new("textures/console.png", false);
    Texture t8 = texture_new("textures/wall_damage_c.png", false);
    Texture t9 = texture_new("textures/test.png", false);
    Texture *textures[9] = {&t1, &t2, &t3, &t4, &t5, &t6, &t7, &t8, &t9};

    Grid grid = grid_load("maps/map_01");

    const double freq_ms       = SDL_GetPerformanceFrequency();
    Uint64       last_time     = SDL_GetPerformanceCounter();
    unsigned int frame_counter = 0;
    double       frame_timer   = last_time;
    const double frame_delta   = 1000.0 / App.fps;

    while (App.run_status) {
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

        if (delta > frame_delta) {
            /* clear screen */
            glClearColor(.0f, .0f, .0f, .0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            /* update here */
            update(&App, &keys_map);

            /* draw here */
            draw(&App, &window_deamon, textures);

            {
                texture_bind(t9);
                draw_rectf((Rectanglef){30, 30, 200, 200}, WHITE);
                texture_unbind();
            }

            window_finish_frame(&window_deamon);

            last_time = current_time;
            ++frame_counter;
        }
    }

    /* shutdown */
    window_shutdown(&window_deamon);
    return 0;
}
