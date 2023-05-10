#include <GL/gl.h>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_opengl.h>
#include <math.h>
#include <time.h>
#include <string>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

#include "engine.hpp"

int TEXTURE_LEN = 8;
unsigned int TEXTURE[8];

int map[24][24] = { { 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 4, 6, 4, 4, 6, 4, 6, 4, 4, 4, 6, 4 },
                    { 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4 },
                    { 8, 0, 3, 3, 0, 0, 0, 0, 0, 8, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
                    { 8, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
                    { 8, 0, 3, 3, 0, 0, 0, 0, 0, 8, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4 },
                    { 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 4, 0, 0, 0, 0, 0, 6, 6, 6, 0, 6, 4, 6 },
                    { 8, 8, 8, 8, 0, 8, 8, 8, 8, 8, 8, 4, 4, 4, 4, 4, 4, 6, 0, 0, 0, 0, 0, 6 },
                    { 7, 7, 7, 7, 0, 7, 7, 7, 7, 0, 8, 0, 8, 0, 8, 0, 8, 4, 0, 4, 0, 6, 0, 6 },
                    { 7, 7, 0, 0, 0, 0, 0, 0, 7, 8, 0, 8, 0, 8, 0, 8, 8, 6, 0, 0, 0, 0, 0, 6 },
                    { 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 6, 0, 0, 0, 0, 0, 4 },
                    { 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 6, 0, 6, 0, 6, 0, 6 },
                    { 7, 7, 0, 0, 0, 0, 0, 0, 7, 8, 0, 8, 0, 8, 0, 8, 8, 6, 4, 6, 0, 6, 6, 6 },
                    { 7, 7, 7, 7, 0, 7, 7, 7, 7, 8, 8, 4, 0, 6, 8, 4, 8, 3, 3, 3, 0, 3, 3, 3 },
                    { 2, 2, 2, 2, 0, 2, 2, 2, 2, 4, 6, 4, 0, 0, 6, 0, 6, 3, 0, 0, 0, 0, 0, 3 },
                    { 2, 2, 0, 0, 0, 0, 0, 2, 2, 4, 0, 0, 0, 0, 0, 0, 4, 3, 0, 0, 0, 0, 0, 3 },
                    { 2, 0, 0, 0, 0, 0, 0, 0, 2, 4, 0, 0, 0, 0, 0, 0, 4, 3, 0, 0, 0, 0, 0, 3 },
                    { 1, 0, 0, 0, 0, 0, 0, 0, 1, 4, 4, 4, 4, 4, 6, 0, 6, 3, 3, 0, 0, 0, 3, 3 },
                    { 2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 1, 2, 2, 2, 6, 6, 0, 0, 5, 0, 5, 0, 5 },
                    { 2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 2, 2, 0, 5, 0, 5, 0, 0, 0, 5, 5 },
                    { 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 2, 5, 0, 5, 0, 5, 0, 5, 0, 5 },
                    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5 },
                    { 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 2, 5, 0, 5, 0, 5, 0, 5, 0, 5 },
                    { 2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 2, 2, 0, 5, 0, 5, 0, 0, 0, 5, 5 },
                    { 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 5, 5, 5, 5, 5, 5, 5, 5, 5 } };

SDL_Window *sdl_window = NULL;
SDL_GLContext sdl_gl_context;

float normalize_rand(float rand)
{
        if (rand > PI2)
                return rand - PI2;
        if (rand < 0)
                return rand + PI2;
        return rand;
}

void update_player(AppGame *App)
{
        float magntude = 1;
        if (App->Player.Buttons.shift == 1)
                magntude = 2;

        /* FIX TO USE THE RELATIVE X/Y REF BASED ON THE PLAYER VIEW */
        if (App->Player.Buttons.d == 1) {
                App->Player.y -= App->Player.direction_x * magntude;
        }
        if (App->Player.Buttons.a == 1) {
                App->Player.y += App->Player.direction_x * magntude;
        }
        if (App->Player.Buttons.w == 1) {
                App->Player.x += App->Player.direction_x * magntude;
                App->Player.y += App->Player.direction_y * magntude;
        }
        if (App->Player.Buttons.s == 1) {
                App->Player.x -= App->Player.direction_x * magntude;
                App->Player.y -= App->Player.direction_y * magntude;
        }
        if (App->Player.Buttons.j == 1) {
                App->Player.pitch_view -= 10;
        }
        if (App->Player.Buttons.k == 1) {
                App->Player.pitch_view += 10;
        }
        App->Player.angle = normalize_rand(App->Player.angle);
}

static void handle_key(SDL_Keysym keysym, AppGame *App, int button_action)
{
        switch (keysym.sym) {
        case SDLK_ESCAPE:
                App->run_status = 0;
                break;
        case SDLK_a:
                App->Player.Buttons.a = button_action;
                break;
        case SDLK_d:
                App->Player.Buttons.d = button_action;
                break;
        case SDLK_w:
                App->Player.Buttons.w = button_action;
                break;
        case SDLK_s:
                App->Player.Buttons.s = button_action;
                break;
        case SDLK_LSHIFT:
                App->Player.Buttons.shift = button_action;
                break;
        case SDLK_j:
                App->Player.Buttons.j = button_action;
                App->Player.Buttons.k = 0;
                break;
        case SDLK_k:
                App->Player.Buttons.k = button_action;
                App->Player.Buttons.j = 0;
                break;
        case SDLK_LCTRL:
                App->Player.Buttons.ctrl = button_action;
                break;
        }
}

void engine_SDL_OpenGL_setup(AppGame *App)
{
        int VSYNC = SDL_DISABLE;

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

        sdl_window = SDL_CreateWindow(App->window_name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      App->screen_width, App->screen_heigh, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
        if (sdl_window == NULL) {
                perror(SDL_GetError());
        }

        sdl_gl_context = SDL_GL_CreateContext(sdl_window);
        if (sdl_gl_context == NULL) {
                perror(SDL_GetError());
        }

        /* setup vsync */
        if (SDL_GL_SetSwapInterval(VSYNC) < 0) {
                perror(SDL_GetError());
        }

        /* init opengl view */
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, App->screen_width, App->screen_heigh, 0, 1, -1);
        glMatrixMode(GL_MODELVIEW);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        if (App->window_fullcreen)
                SDL_SetWindowFullscreen(sdl_window, SDL_WINDOW_FULLSCREEN);

        SDL_ShowCursor(SDL_DISABLE);
        SDL_CaptureMouse(SDL_TRUE);
        SDL_SetRelativeMouseMode(SDL_TRUE);
}

bool check_map_bound_index(int index_x, int index_y, int index_max_x, int index_max_y)
{
        if (index_x >= 0 && index_x < index_max_x && index_y >= 0 && index_y < index_max_y)
                return true;
        else
                return false;
}

typedef struct {
        glm::ivec2 start_point;
        glm::fvec2 uvec_direction_view;
        enum { RIGTH, LEFT } side;
        glm::ivec2 map_index;
} DDA;

void DDA_Algorith(AppGame *App, glm::fvec2 *point_collision_map, glm::fvec2 *point_direction, glm::fvec2 point_start,
                  int *side, int *map_index_value, float *dist, float ray_total_max = 1000.0)
{
        float ray_total = 0.0;
        bool ray_bound = false;

        glm::fvec2 delta = glm::fvec2(point_direction->x / App->map_height - point_start.x / App->map_height,
                                      point_direction->y / App->map_height - point_start.y / App->map_height);
        glm::fvec2 unitary_vector = glm::fvec2(delta.x / glm::length(delta), delta.y / glm::length(delta));
        glm::fvec2 unitary_step_size = glm::fvec2(sqrtf(1 + powf(unitary_vector.y / unitary_vector.x, 2)),
                                                  sqrtf(1 + powf(unitary_vector.x / unitary_vector.y, 2)));
        glm::fvec2 ray_length_1D, step;
        glm::ivec2 tile_map_check = glm::ivec2(point_start.x, point_start.y);
        glm::ivec2 absolute_map_tile;

        /* set x walk */
        if (unitary_vector.x < 0) {
                step.x = -1;
                ray_length_1D.x = (point_start.x - tile_map_check.x) * unitary_step_size.x;
        } else {
                step.x = 1;
                ray_length_1D.x = ((tile_map_check.x + 1) - point_start.x) * unitary_step_size.x;
        }

        /* set y walk */
        if (unitary_vector.y < 0) {
                step.y = -1;
                ray_length_1D.y = (point_start.y - tile_map_check.y) * unitary_step_size.y;
        } else {
                step.y = 1;
                ray_length_1D.y = ((tile_map_check.y + 1) - point_start.y) * unitary_step_size.y;
        }

        while (!ray_bound && ray_total < ray_total_max) {
                /* walk */
                if (ray_length_1D.x < ray_length_1D.y) {
                        tile_map_check.x += step.x;
                        ray_total = ray_length_1D.x;
                        ray_length_1D.x += unitary_step_size.x;
                        *side = 0; /* vertical */
                } else {
                        tile_map_check.y += step.y;
                        ray_total = ray_length_1D.y;
                        ray_length_1D.y += unitary_step_size.y;
                        *side = 1; /* horizontal */
                }

                /* check */
                absolute_map_tile.x = tile_map_check.x / App->map_height;
                absolute_map_tile.y = tile_map_check.y / App->map_height;
                if (check_map_bound_index(absolute_map_tile.x, absolute_map_tile.y, App->map_cols, App->map_rows) ==
                            1 &&
                    App->map_tile[absolute_map_tile.y][absolute_map_tile.x] != 0) {
                        ray_bound = true;
                        *map_index_value = App->map_tile[absolute_map_tile.y][absolute_map_tile.x];
                }
        }

        if (ray_bound == true) {
                point_collision_map->x = point_start.x + unitary_vector.x * ray_total;
                point_collision_map->y = point_start.y + unitary_vector.y * ray_total;
        }
}

void engine_SDL_OpenGL_shutdown()
{
        SDL_VideoQuit();
        SDL_GL_DeleteContext(sdl_gl_context);
        SDL_DestroyWindow(sdl_window);
        SDL_Quit();
}

void handle_mouse_pressed_down(int button, float x, float y, AppGame *App)
{
        switch (button) {
        case SDL_BUTTON_LEFT:
                break;
        case SDL_BUTTON_RIGHT:
                break;
        }
}

/* TODO move to shader */
void draw_aim(AppGame *App)
{
        float pixel[3];

        glLogicOp(GL_COPY_INVERTED);
        glEnable(GL_COLOR_LOGIC_OP);
        glBegin(GL_POINTS);
        for (int i = 0; i < 20; i++) {
                int x = App->screen_width / 2;
                int y = (App->screen_heigh / 2) - 10;

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
                int x = (App->screen_width / 2) - 10;
                int y = (App->screen_heigh / 2);

                glReadPixels(x + i, y, 1, 1, GL_RGB, GL_FLOAT, &pixel);
                glColor3f(pixel[0], pixel[1], pixel[2]);
                glVertex2i(x + i, y);
        }
        glEnd();
        glLogicOp(GL_COPY);
        glDisable(GL_COLOR_LOGIC_OP);
}

void handle_mouse_pressed_up(int button, float x, float y, AppGame *App)
{
        switch (button) {
        case SDL_BUTTON_LEFT:
                break;
        case SDL_BUTTON_RIGHT:
                break;
        }
}

void load_textures(unsigned int texture, string texture_name)
{
        stbi_set_flip_vertically_on_load(1);

        int f_width, f_height, bpp;
        unsigned char *data = stbi_load(texture_name.c_str(), &f_width, &f_height, &bpp, 0);
        if (!data) {
                SDL_Log("load data error");
        }

        /* init texture from file pixels to GPU MEMORY*/
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                        GL_CLAMP_TO_BORDER); // set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, f_width, f_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
}

/* move from float to double */
void draw_3d_view_port(AppGame *App)
{
        int wall_high = 64, pixels_cols = App->screen_width;
        double fov_in_rad = App->Player.fov * ONE_RAD, hfov_in_rad = (App->Player.fov / 2) * ONE_RAD;
        double pixel_in_rad = fov_in_rad / pixels_cols;
        int draw_screen_h = App->screen_heigh;

        int pitch = App->Player.pitch_view; /* player look up or down */
        glm::fvec2 point_start = glm::fvec2(App->Player.x, App->Player.y);

        for (int pixel = 0; pixel < pixels_cols; pixel++) {
                double angle =
                        normalize_rand(normalize_rand(App->Player.angle - hfov_in_rad) + (pixel_in_rad * pixel));
                glm::fvec2 point_end = glm::fvec2(App->Player.x + cos(angle), App->Player.y + -sin(angle));
                glm::fvec2 collision_wall;
                int side, map_index_value;
                float d2;

                DDA_Algorith(App, &collision_wall, &point_end, point_start, &side, &map_index_value, &d2);

                if (collision_wall.x != 0 && collision_wall.y != 0) {
                        float d = glm::length(point_start - collision_wall);
                        d = d * cosf(App->Player.angle - angle); /* fix eye fish */

                        int line_h = (App->map_height * draw_screen_h) / (d);
                        int line_start = ((draw_screen_h / 2) - (line_h / 2)) - pitch;
                        int line_end = line_start + line_h;

                        double wall_hit;
                        if (side == 1) {
                                /* for some reason wall_high is divived by 2 */
                                wall_hit = double(int(collision_wall.x) % (wall_high / 2)) / (wall_high / 2);
                        } else {
                                wall_hit = double(int(collision_wall.y) % (wall_high / 2)) / (wall_high / 2);
                        }

                        draw_vertical_view(App->texture[map_index_value - 1], wall_hit, line_start, line_end, pixel);
                }
        }
}

void draw_3d_view_flor_and_ceil(AppGame *App)
{
        int center_h = App->screen_heigh / 2;
        int pitch = App->Player.pitch_view;

        glBegin(GL_QUADS);
        glColor3f(.0, .0, .0);
        glVertex2i(0, 0);
        glVertex2i(App->screen_width, 0);
        glVertex2i(App->screen_width, center_h + pitch);
        glVertex2i(0, center_h + pitch);

        glColor3f(.4, .4, .4);
        glVertex2i(0, center_h - pitch);
        glVertex2i(App->screen_width, center_h - pitch);
        glVertex2i(App->screen_width, App->screen_heigh);
        glVertex2i(0, App->screen_heigh);
        glEnd();
}

void draw(AppGame *App)
{
        draw_3d_view_flor_and_ceil(App);
        draw_3d_view_port(App);
        draw_aim(App);
}

void handle_mouse_motion(AppGame *App, SDL_Event *event)
{
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

                App->Player.angle += (ONE_RAD / 6) * event->motion.xrel;
                App->Player.direction_x = cos(App->Player.angle);
                App->Player.direction_y = -sin(App->Player.angle);

                break;
        }
}

void engine_SDL_OpenGL_load_textures(AppGame *App)
{
        glGenTextures(TEXTURE_LEN, TEXTURE);

        load_textures(TEXTURE[0], "./textures/eagle.png");
        load_textures(TEXTURE[1], "./textures/redbrick.png");
        load_textures(TEXTURE[2], "./textures/purplestone.png");
        load_textures(TEXTURE[3], "./textures/greystone.png");
        load_textures(TEXTURE[4], "./textures/bluestone.png");
        load_textures(TEXTURE[5], "./textures/mossy.png");
        load_textures(TEXTURE[6], "./textures/wood.png");
        load_textures(TEXTURE[7], "./textures/colorstone.png");
        App->texture = TEXTURE;
}

int main(int argc, char *args[])
{
        srand(time(NULL));
        AppGame App = { 1920, 1080, "Simple Wolfenstein Engine", 1, 0, { 300, 300, 0, cos(PI2), -sin(PI2), 0, 70 }, 24,
                        24,   32 };
        for (int x = 0; x < App.map_cols; x++)
                for (int y = 0; y < App.map_rows; y++)
                        App.map_tile[x][y] = map[x][y];

        engine_SDL_OpenGL_setup(&App);
        engine_SDL_OpenGL_load_textures(&App);

        while (App.run_status) {
                SDL_Event event;
                while (SDL_PollEvent(&event) != 0) {
                        switch (event.type) {
                        case SDL_QUIT:
                                App.run_status = 0;
                                break;
                        case SDL_KEYDOWN:
                                handle_key(event.key.keysym, &App, 1);
                                break;
                        case SDL_KEYUP:
                                handle_key(event.key.keysym, &App, 0);
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

                /* clear screen */
                glClearColor(.0f, .0f, .0f, .0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                /* update here */
                update_player(&App);

                /* draw here */
                draw(&App);

                /* update screen */
                SDL_GL_SwapWindow(sdl_window);
        }

        /* shutdown */
        engine_SDL_OpenGL_shutdown();
        return 0;
}
