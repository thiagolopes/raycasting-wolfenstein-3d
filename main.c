#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_opengl.h>
#include <math.h>
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define PI 3.14
#define PI2 (PI * 2)
#define ONE_RAD PI / 180

#define FOV 70
#define hFOV FOV / 2
#define W 1920
#define H 1080

#define INT(x) ((int)x)

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
typedef struct {
        float x, y;
} Pointf;

typedef struct {
        int x, y;
} Point;

Pointf MOUSE_POINT;
int MOUSE_POINT_SHOW;
Pointf INTERSECTION_POINT;

typedef struct {
        Point p1, p2;
        float length;
} Vector;

typedef struct {
        int a, w, s, d, shift, ctrl, j, k;
} ButtonKeys;

typedef struct {
        float x, y, xref, yref, button_r, button_l;
} Mouse;

typedef struct {
        float x, y;
        double angle, direction_x, direction_y;
        int pitch_view;
        ButtonKeys Buttons;
} Player;

typedef struct {
        int screen_width;
        int screen_heigh;
        char *window_name;
        int run_status;
        int window_fullcreen;
        Player Player;

        /* move to map */
        int map_cols;
        int map_rows;
        int map_height;
        int map_tile[32][32];

        /* engine */
        unsigned int *texture;
} AppGame;

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

        if (App->Player.Buttons.d == 1) {
                App->Player.angle += ONE_RAD;
                App->Player.direction_x = cos(App->Player.angle);
                App->Player.direction_y = -sin(App->Player.angle);
        }
        if (App->Player.Buttons.a == 1) {
                App->Player.angle -= ONE_RAD;
                App->Player.direction_x = cos(App->Player.angle);
                App->Player.direction_y = -sin(App->Player.angle);
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

float dist(float ax, float ay, float bx, float by)
{
        return sqrt((bx - ax) * (bx - ax) + (by - ay) * (by - ay));
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
        }

        SDL_Log("a=%i, d%i, w=%i, s=%i", App->Player.Buttons.a, App->Player.Buttons.d, App->Player.Buttons.w,
                App->Player.Buttons.s);
}

void SDLOpenGLSetup(AppGame App)
{
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVERYTHING | SDL_INIT_NOPARACHUTE) < 0) {
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

        sdl_window = SDL_CreateWindow(App.window_name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      App.screen_width, App.screen_heigh, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
        if (sdl_window == NULL) {
                perror(SDL_GetError());
        }

        sdl_gl_context = SDL_GL_CreateContext(sdl_window);
        if (sdl_gl_context == NULL) {
                perror(SDL_GetError());
        }

        /* setup vsync */
        if (SDL_GL_SetSwapInterval(1) < 0) {
                perror(SDL_GetError());
        }

        /* init opengl view */
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, App.screen_width, App.screen_heigh, 0, 1, -1);
        glMatrixMode(GL_MODELVIEW);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        if (App.window_fullcreen)
                SDL_SetWindowFullscreen(sdl_window, SDL_WINDOW_FULLSCREEN);

        SDL_ShowCursor(0);
}

void draw_dot(float x, float y)
{
        glColor3f(1, 1, 1);
        glPointSize(1);
        glBegin(GL_POINTS);
        glVertex2i(x, y);
        glEnd();
}

int check_map_bound_index(int index_x, int index_y, int index_max_x, int index_max_y)
{
        if (index_x >= 0 && index_x < index_max_x && index_y >= 0 && index_y < index_max_y)
                return 1;
        else
                return 0;
}

void DDA_Algorith(AppGame *App, Pointf *point_collision_map, Pointf *point_direction, int *side, int *map_value,
                  float *dist)
{
        int bound = 0;
        float ray_dist = 0;
        float ray_dist_max_walk = 1000.0;

        Pointf point_start = { App->Player.x, App->Player.y },
               delta = { point_direction->x / App->map_height - point_start.x / App->map_height,
                         point_direction->y / App->map_height - point_start.y / App->map_height };

        float point_magnitude = sqrtf(powf(delta.x, 2) + powf(delta.y, 2));

        Pointf unitary_vector = { delta.x / point_magnitude, delta.y / point_magnitude },
               unitary_step_size = { sqrtf(1 + powf(unitary_vector.y / unitary_vector.x, 2)),
                                     sqrtf(1 + powf(unitary_vector.x / unitary_vector.y, 2)) },
               ray_length_1D, step;

        Pointf tile_map_check = { truncf(point_start.x), truncf(point_start.y) };

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

        while (!bound && ray_dist < ray_dist_max_walk) {
                /* walk */
                if (ray_length_1D.x < ray_length_1D.y) {
                        tile_map_check.x += step.x;
                        ray_dist = ray_length_1D.x;
                        ray_length_1D.x += unitary_step_size.x;
                        *side = 0; /* vertical todo verify */
                } else {
                        tile_map_check.y += step.y;
                        ray_dist = ray_length_1D.y;
                        ray_length_1D.y += unitary_step_size.y;
                        *side = 1; /* horizontal */
                }

                /* check */
                Point absolute_map_tile = { tile_map_check.x / App->map_height, tile_map_check.y / App->map_height };
                if (check_map_bound_index(absolute_map_tile.x, absolute_map_tile.y, App->map_cols, App->map_rows) ==
                            1 &&
                    App->map_tile[absolute_map_tile.y][absolute_map_tile.x] != 0) {
                        bound = 1;
                        *map_value = App->map_tile[absolute_map_tile.y][absolute_map_tile.x];
                }
        }

        if (bound == 1) {
                /* point_collision_map->x = tile_map_check.x; */
                /* point_collision_map->y = tile_map_check.y; */

                point_collision_map->x = point_start.x + unitary_vector.x * ray_dist;
                point_collision_map->y = point_start.y + unitary_vector.y * ray_dist;
        }
}

void SDLOpenGLShutdown()
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
                App->map_tile[INT(truncf(y)) / App->map_height][INT(truncf(x)) / App->map_height] = 1;
                break;
        case SDL_BUTTON_RIGHT:
                MOUSE_POINT_SHOW = 1;
                MOUSE_POINT.x = x;
                MOUSE_POINT.y = y;
                break;
        }
}

void draw_center(AppGame *App)
{
        glColor3f(1, 1, 1);
        glLineWidth(1);
        glBegin(GL_LINES);
        glVertex2i((App->screen_width / 2), (App->screen_heigh / 2) - (5));
        glVertex2i((App->screen_width / 2), (App->screen_heigh / 2) + (5));

        float off = (5 * 1.7) / 2;
        glVertex2i((App->screen_width / 2) - off, (App->screen_heigh / 2));
        glVertex2i((App->screen_width / 2) + off, (App->screen_heigh / 2));
        glEnd();
}

void handle_mouse_pressed_up(int button, float x, float y, AppGame *App)
{
        switch (button) {
        case SDL_BUTTON_LEFT:
                break;
        case SDL_BUTTON_RIGHT:
                MOUSE_POINT_SHOW = 0;
                break;
        }
}

void load_textures(AppGame *App, unsigned int *texture, char *texture_name)
{
        stbi_set_flip_vertically_on_load(1);

        int f_width, f_height, bpp;
        unsigned char *data = stbi_load(texture_name, &f_width, &f_height, &bpp, 0);
        if (!data) {
                SDL_Log("load data error");
        }
        /* init texture from file */
        glBindTexture(GL_TEXTURE_2D, *texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                        GL_CLAMP_TO_BORDER); // set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, f_width, f_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
}

/* move from float to double */
void draw_3d_view_port(int fov, AppGame *App)
{
        int wall_high = 64, pixels_cols = App->screen_width;
        double fov_in_rad = fov * ONE_RAD, hfov_in_rad = (fov / 2) * ONE_RAD;
        double pixel_in_rad = fov_in_rad / pixels_cols;

        int pitch = App->Player.pitch_view; /* player look up or down */

        for (int i = 0; i < pixels_cols; i++) {
                double angle = normalize_rand(normalize_rand(App->Player.angle - hfov_in_rad) + (pixel_in_rad * i));
                Pointf point_end = { App->Player.x + cos(angle), App->Player.y + -sin(angle) };
                Pointf collision_wall = { 0, 0 };
                int side;
                int map_value;
                float d2;

                DDA_Algorith(App, &collision_wall, &point_end, &side, &map_value, &d2);

                if (collision_wall.x != 0 && collision_wall.y != 0) {
                        double d = dist(App->Player.x, App->Player.y, collision_wall.x, collision_wall.y);
                        d = d * cosf(App->Player.angle - angle); /* fix eye fish */

                        int draw_screen_h = App->screen_heigh;
                        int line_h = (App->map_height * draw_screen_h) / (d);
                        int line_start = ((draw_screen_h / 2) - (line_h / 2)) - pitch;
                        int line_end = line_start + line_h;

                        double wall_hit;
                        if (side == 1) {
                                /* for some reason wall_high is divived by 2 */
                                wall_hit = (double)((int)collision_wall.x % (wall_high / 2)) / (wall_high / 2);
                        } else {
                                wall_hit = (double)((int)collision_wall.y % (wall_high / 2)) / (wall_high / 2);
                        }

                        /* 3drender here, TODO move  */
                        glColor3f(1, 1, 1);
                        glEnable(GL_TEXTURE_2D);
                        glBindTexture(GL_TEXTURE_2D, App->texture[map_value - 1]);
                        glBegin(GL_LINES);

                        glTexCoord2f(wall_hit, 1.0);
                        glVertex2i(i, line_start);

                        glTexCoord2f(wall_hit, 0.0);
                        glVertex2i(i, line_end);

                        glEnd();
                        glDisable(GL_TEXTURE_2D);
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
        draw_3d_view_port(FOV, App);
        draw_center(App);
}

int main(int argc, char *args[])
{
        srand(time(NULL));
        AppGame App = { W, H, "Project raycasting", 1, 0, { 300, 300, cos(PI2), -sin(PI2), PI2, 0 } };
        App.map_cols = 24;
        App.map_rows = 24;
        App.map_height = 32;
        for (int x = 0; x < App.map_cols; x++)
                for (int y = 0; y < App.map_rows; y++)
                        App.map_tile[x][y] = map[x][y];

        SDLOpenGLSetup(App);

        glGenTextures(8, TEXTURE);

        load_textures(&App, &TEXTURE[0], "./textures/eagle.png");
        load_textures(&App, &TEXTURE[1], "./textures/redbrick.png");
        load_textures(&App, &TEXTURE[2], "./textures/purplestone.png");
        load_textures(&App, &TEXTURE[3], "./textures/greystone.png");
        load_textures(&App, &TEXTURE[4], "./textures/bluestone.png");
        load_textures(&App, &TEXTURE[5], "./textures/mossy.png");
        load_textures(&App, &TEXTURE[6], "./textures/wood.png");
        load_textures(&App, &TEXTURE[7], "./textures/colorstone.png");
        App.texture = TEXTURE;

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
                                if (event.motion.state == (SDL_BUTTON_LMASK | SDL_BUTTON_RMASK)) {
                                        handle_mouse_pressed_down(SDL_BUTTON_LEFT, event.button.x, event.button.y,
                                                                  &App);
                                        handle_mouse_pressed_down(SDL_BUTTON_RIGHT, event.button.x, event.button.y,
                                                                  &App);
                                } else if (event.motion.state == SDL_BUTTON_LMASK) {
                                        handle_mouse_pressed_down(SDL_BUTTON_LEFT, event.button.x, event.button.y,
                                                                  &App);
                                } else if (event.motion.state == SDL_BUTTON_RMASK) {
                                        handle_mouse_pressed_down(SDL_BUTTON_RIGHT, event.button.x, event.button.y,
                                                                  &App);
                                }
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
        SDLOpenGLShutdown();
        return 0;
}
