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
#include "main.h"
#include "colors.h"
#include "dda.c"

// imgui
// #include "external/imgui/backends/imgui_impl_opengl3.h"
// #include "external/imgui/backends/imgui_impl_sdl2.h"
// #include "external/imgui/imgui.h"

// stb
#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

void draw_rect(Rectanglef rect, Color color) {
    glBegin(GL_QUADS);
    glColor4ub(color.r, color.g, color.b, color.a);
    glVertex2i(rect.x, rect.y);
    glVertex2i(rect.x + rect.width, rect.y);
    glVertex2i(rect.x + rect.width, rect.y + rect.height);
    glVertex2i(rect.x, rect.y + rect.height);
    glEnd();
}

void draw_vertical_line(float posX, float posY, float posY_end, int texture_id, float texture_offset, Color color) {
    glColor4ub(color.r, color.g, color.b, color.a);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glBegin(GL_LINES);

    glTexCoord2d(texture_offset, 1.0);
    glVertex2i(posX, posY);

    glTexCoord2d(texture_offset, 0.0);
    glVertex2i(posX, posY_end);

    glEnd();

    glDisable(GL_TEXTURE_2D);
}

// methods
void update_player(Keys* keys, Player_t* player) {
    float magntude = 2;
    if (keys->shift == 1)
        magntude = 4;

    /* FIX TO USE THE RELATIVE X/Y REF BASED ON THE PLAYER VIEW */
    if (keys->d == 1) {
        player->y -= player->direction_x * magntude;
    }
    if (keys->a == 1) {
        player->y += player->direction_x * magntude;
    }
    if (keys->w == 1) {
        player->x += player->direction_x * magntude;
        player->y += player->direction_y * magntude;
    }
    if (keys->s == 1) {
        player->x -= player->direction_x * magntude;
        player->y -= player->direction_y * magntude;
    }
    if (keys->j == 1) {
        player->pitch_view -= 10;
    }
    if (keys->k == 1) {
        player->pitch_view += 10;
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


void handle_mouse_pressed_down(int button, float x, float y, AppGame *App) {
    switch (button) {
        case SDL_BUTTON_LEFT:
            break;
        case SDL_BUTTON_RIGHT:
            break;
    }
}

/* TODO move to shader */
void draw_aim(Window *window) {
    float pixel[3];

    glLogicOp(GL_COPY_INVERTED);
    glEnable(GL_COLOR_LOGIC_OP);
    glBegin(GL_POINTS);
    for (int i = 0; i < 20; i++) {
        int x = window->width / 2;
        int y = (window->height / 2) - 10;

        glReadPixels(x, y + i, 1, 1, GL_RGB, GL_FLOAT, &pixel);
        glColor3f(pixel[0], pixel[1], pixel[2]);
        glVertex2i(x, y + i);
    }
    glEnd();
    glLogicOp(GL_COPY);
    glDisable(GL_COLOR_LOGIC_OP);

    glLogicOp(GL_COPY_INVERTED);
    glEnable(GL_COLOR_LOGIC_OP);
    glBegin(GL_POINTS);

    for (int i = 0; i < 20; i++) {
        int x = (window->width / 2) - 10;
        int y = (window->height / 2);

        glReadPixels(x + i, y, 1, 1, GL_RGB, GL_FLOAT, &pixel);
        glColor3f(pixel[0], pixel[1], pixel[2]);
        glVertex2i(x + i, y);
    }
    glEnd();
    glLogicOp(GL_COPY);
    glDisable(GL_COLOR_LOGIC_OP);
}

void handle_mouse_pressed_up(int button, float x, float y, AppGame *App) {
    switch (button) {
        case SDL_BUTTON_LEFT:
            break;
        case SDL_BUTTON_RIGHT:
            break;
    }
}

void load_textures(unsigned int texture, char *texture_name) {
    stbi_set_flip_vertically_on_load(1);

    int            f_width, f_height, bpp;
    unsigned char *data = stbi_load(texture_name, &f_width, &f_height, &bpp, STBI_rgb_alpha);
    if (!data) {
        SDL_Log("load data error");
    }

    /* init texture from file pixels to GPU MEMORY*/
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_CLAMP_TO_BORDER); // set texture wrapping to GL_REPEAT (default wrapping
                                         // method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, f_width, f_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
}

void draw_3d_view_port(AppGame *App, Window *win) {
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

        angle       = normalize_rand(normalize_rand(player_angle - hfov_in_rad) + pixel_in_rad * pixel);
        point_end.x = point_start.x + cos(angle);
        point_end.y = point_start.y + -sin(angle);

        ray_collision = DDA(map, wall_height, map_len, point_end, point_start);

        if (ray_collision.valid) {
            distance = fix_eye_fish(point2f_sub(point_start, ray_collision.collision_point), player_angle - angle);

            line_h     = (wall_height * draw_screen_h) / distance;
            line_start = (draw_screen_h / 2 - line_h / 2) - pitch;
            line_end   = line_start + line_h;

            if (ray_collision.side == HORIZONTAL) {
                draw_color.r   = 255;
                draw_color.g   = 255;
                draw_color.b   = 255;
                texture_offset = (float)((int)(ray_collision.collision_point.x) % wall_height) / wall_height;
            } else {
                draw_color.r   = 255 / fade_draw_vertical;
                draw_color.g   = 255 / fade_draw_vertical;
                draw_color.b   = 255 / fade_draw_vertical;
                texture_offset = (float)((int)(ray_collision.collision_point.y) % wall_height) / wall_height;
            }

            draw_vertical_line(pixel, line_start, line_end, ray_collision.grid_index_collision, texture_offset,
                               draw_color);
        }
    }
}

void draw_3d_view_floor(AppGame* App, Window* win) {
    float      floor_start = win->height / 2 - App->Player.pitch_view;
    Rectanglef floor = {0, floor_start, (float)win->width, (float)win->height + App->Player.pitch_view};
    draw_rect(floor, DARKGRAY);
}

void draw(AppGame* App, Window* win) {
    draw_3d_view_floor(App, win);
    draw_3d_view_port(App, win);
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

void engine_SDL_OpenGL_load_textures(AppGame *App) {
    char texture_dir[100] = "./textures/";
    // setup texture memory
    App->texture_len = 0;

    DIR           *textures_dirs;
    struct dirent *ent;
    if ((textures_dirs = opendir(texture_dir)) != NULL) {
        while ((ent = readdir(textures_dirs)) != NULL) {
            App->texture_len += 1;
        }
    } else {
        // error
    }

    App->texture = (unsigned int *)malloc(sizeof(unsigned int) * App->texture_len);
    glGenTextures(App->texture_len, App->texture);

    // load texture
    char dir_cat[100];
    seekdir(textures_dirs, 0);
    for (int i = (App->texture_len - 1); i >= 0; i--) {
        ent = readdir(textures_dirs);
        if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..")) {
            strcpy(dir_cat, "");
            strcpy(dir_cat, texture_dir);
            strcat(dir_cat, ent->d_name);
            printf("%s\n", dir_cat);
            load_textures(App->texture[i], dir_cat);
        }
    }
    closedir(textures_dirs);
    free(App->texture);
}

// main
int main(int argc, char *args[]) {
    srand(time(NULL));
    char    title_format[] = "Simple Wolfenstein Engine - FPS %i";
    char    title[256]     = "Simple Wolfenstein Engine";
    AppGame App            = {1, {300, 300, 0, cos(TAU), -sin(TAU), 0, 70}, 24, 24, 32};
    App.fps                = 60;
    for (int x = 0; x < App.map_cols; x++)
        for (int y = 0; y < App.map_rows; y++)
            App.map_tile[x][y] = map[x][y];

    Window window_deamon = window_wake_up(title, 1920, 1080, false);
    Keys keys_map;
    Mouse mouse;
    window_vsync(true);
    window_keep_mouse_on_center(true);
    engine_SDL_OpenGL_load_textures(&App);

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
            draw(&App, &window_deamon);
            window_finish_frame(&window_deamon);

            last_time = current_time;
            ++frame_counter;
        }
    }

    /* shutdown */
    window_shutdown(&window_deamon);
    return 0;
}
