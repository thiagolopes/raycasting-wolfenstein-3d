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

#include <glm/glm.hpp>
#include <iostream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "dda.cpp"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_sdl2.h"
#include "imgui/imgui.h"
#include "stb_image.h"

#define PI 3.141592
#define PI2 6.283185
#define ONE_RAD 0.0174533

// definitions
typedef struct {
    int a, w, s, d, shift, ctrl, j, k;
} ButtonKeys;

typedef struct {
    float x, y, xref, yref, button_r, button_l;
} Mouse;

typedef struct {
    float  x, y;
    double angle, direction_x, direction_y;
    int    pitch_view, fov;
} PLAYER;

typedef struct {
    int         screen_width;
    int         screen_heigh;
    std::string window_name;
    int         run_status;
    int         window_fullcreen;
    PLAYER      Player;

    /* move to map */
    int map_cols;
    int map_rows;
    int map_height;
    int map_tile[24][24];

    /* engine */
    unsigned int *texture;
    ButtonKeys    Buttons;
} AppGame;

int          TEXTURE_LEN = 8;
unsigned int TEXTURE[8];

int map[24][24] = {{8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 4, 6, 4, 4, 6, 4, 6, 4, 4, 4, 6, 4},
                   {8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
                   {8, 0, 3, 3, 0, 0, 0, 0, 0, 8, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6},
                   {8, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6},
                   {8, 0, 3, 3, 0, 0, 0, 0, 0, 8, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
                   {8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 4, 0, 0, 0, 0, 0, 6, 6, 6, 0, 6, 4, 6},
                   {8, 8, 8, 8, 0, 8, 8, 8, 8, 8, 8, 4, 4, 4, 4, 4, 4, 6, 0, 0, 0, 0, 0, 6},
                   {7, 7, 7, 7, 0, 7, 7, 7, 7, 0, 8, 0, 8, 0, 8, 0, 8, 4, 0, 4, 0, 6, 0, 6},
                   {7, 7, 0, 0, 0, 0, 0, 0, 7, 8, 0, 8, 0, 8, 0, 8, 8, 6, 0, 0, 0, 0, 0, 6},
                   {7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 6, 0, 0, 0, 0, 0, 4},
                   {7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 6, 0, 6, 0, 6, 0, 6},
                   {7, 7, 0, 0, 0, 0, 0, 0, 7, 8, 0, 8, 0, 8, 0, 8, 8, 6, 4, 6, 0, 6, 6, 6},
                   {7, 7, 7, 7, 0, 7, 7, 7, 7, 8, 8, 4, 0, 6, 8, 4, 8, 3, 3, 3, 0, 3, 3, 3},
                   {2, 2, 2, 2, 0, 2, 2, 2, 2, 4, 6, 4, 0, 0, 6, 0, 6, 3, 0, 0, 0, 0, 0, 3},
                   {2, 2, 0, 0, 0, 0, 0, 2, 2, 4, 0, 0, 0, 0, 0, 0, 4, 3, 0, 0, 0, 0, 0, 3},
                   {2, 0, 0, 0, 0, 0, 0, 0, 2, 4, 0, 0, 0, 0, 0, 0, 4, 3, 0, 0, 0, 0, 0, 3},
                   {1, 0, 0, 0, 0, 0, 0, 0, 1, 4, 4, 4, 4, 4, 6, 0, 6, 3, 3, 0, 0, 0, 3, 3},
                   {2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 1, 2, 2, 2, 6, 6, 0, 0, 5, 0, 5, 0, 5},
                   {2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 2, 2, 0, 5, 0, 5, 0, 0, 0, 5, 5},
                   {2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 2, 5, 0, 5, 0, 5, 0, 5, 0, 5},
                   {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5},
                   {2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 2, 5, 0, 5, 0, 5, 0, 5, 0, 5},
                   {2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 2, 2, 0, 5, 0, 5, 0, 0, 0, 5, 5},
                   {2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 5, 5, 5, 5, 5, 5, 5, 5, 5}};

SDL_Window   *sdl_window = nullptr;
SDL_GLContext sdl_gl_context;

// methods
void draw_vertical_view(int texture, double texture_of, int pixel_start, int pixel_end, int vertical_pixel) {
    glColor3f(1, 1, 1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBegin(GL_LINES);

    glTexCoord2d(texture_of, 1.0);
    glVertex2i(vertical_pixel, pixel_start);

    glTexCoord2d(texture_of, 0.0);
    glVertex2i(vertical_pixel, pixel_end);

    glEnd();
    glDisable(GL_TEXTURE_2D);
}

float normalize_rand(float rand) {
    if (rand > PI2)
        return rand - PI2;
    if (rand < 0)
        return rand + PI2;
    return rand;
}

void update_player(AppGame *App) {
    float magntude = 1;
    if (App->Buttons.shift == 1)
        magntude = 2;

    /* FIX TO USE THE RELATIVE X/Y REF BASED ON THE PLAYER VIEW */
    if (App->Buttons.d == 1) {
        App->Player.y -= App->Player.direction_x * magntude;
    }
    if (App->Buttons.a == 1) {
        App->Player.y += App->Player.direction_x * magntude;
    }
    if (App->Buttons.w == 1) {
        App->Player.x += App->Player.direction_x * magntude;
        App->Player.y += App->Player.direction_y * magntude;
    }
    if (App->Buttons.s == 1) {
        App->Player.x -= App->Player.direction_x * magntude;
        App->Player.y -= App->Player.direction_y * magntude;
    }
    if (App->Buttons.j == 1) {
        App->Player.pitch_view -= 10;
    }
    if (App->Buttons.k == 1) {
        App->Player.pitch_view += 10;
    }
    App->Player.angle = normalize_rand(App->Player.angle);
}

void update(AppGame *App) {
    update_player(App);
}

static void handle_key(SDL_Keysym keysym, AppGame *App, int button_action) {
    switch (keysym.sym) {
        case SDLK_ESCAPE:
            App->run_status = 0;
            break;
        case SDLK_a:
            App->Buttons.a = button_action;
            break;
        case SDLK_d:
            App->Buttons.d = button_action;
            break;
        case SDLK_w:
            App->Buttons.w = button_action;
            break;
        case SDLK_s:
            App->Buttons.s = button_action;
            break;
        case SDLK_LSHIFT:
            App->Buttons.shift = button_action;
            break;
        case SDLK_j:
            App->Buttons.j = button_action;
            App->Buttons.k = 0;
            break;
        case SDLK_k:
            App->Buttons.k = button_action;
            App->Buttons.j = 0;
            break;
        case SDLK_LCTRL:
            App->Buttons.ctrl = button_action;
            break;
    }
}

void engine_SDL_OpenGL_setup(AppGame *App) {
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
                                  App->screen_width, App->screen_heigh, SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
    if (sdl_window == nullptr) {
        perror(SDL_GetError());
    }

    sdl_gl_context = SDL_GL_CreateContext(sdl_window);
    if (sdl_gl_context == nullptr) {
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

    SDL_ShowCursor(SDL_ENABLE);
    SDL_CaptureMouse(SDL_TRUE);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

    // imgui
    const unsigned char *glsl   = glGetString(GL_SHADING_LANGUAGE_VERSION);
    std::string          glsl_s = std::to_string(*glsl);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Contro
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(sdl_window, sdl_gl_context);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void engine_SDL_OpenGL_shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_VideoQuit();
    SDL_GL_DeleteContext(sdl_gl_context);
    SDL_DestroyWindow(sdl_window);
    SDL_Quit();
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
void draw_aim(AppGame *App) {
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

void handle_mouse_pressed_up(int button, float x, float y, AppGame *App) {
    switch (button) {
        case SDL_BUTTON_LEFT:
            break;
        case SDL_BUTTON_RIGHT:
            break;
    }
}

void load_textures(unsigned int texture, std::string texture_name) {
    stbi_set_flip_vertically_on_load(1);

    int            f_width, f_height, bpp;
    unsigned char *data = stbi_load(texture_name.c_str(), &f_width, &f_height, &bpp, 0);
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

/* move from float to double */
void draw_3d_view_port(AppGame *App) {
    int    wall_high     = 64;
    int    pixels_cols   = App->screen_width;
    double fov_in_rad    = App->Player.fov * ONE_RAD;
    double hfov_in_rad   = (App->Player.fov / 2) * ONE_RAD;
    double pixel_in_rad  = fov_in_rad / pixels_cols;
    int    draw_screen_h = App->screen_heigh;

    int        pitch       = App->Player.pitch_view; /* player look up or down */
    glm::fvec2 point_start = glm::fvec2(App->Player.x, App->Player.y);

    for (int pixel = 0; pixel < pixels_cols; pixel++) {
        double     angle = normalize_rand(normalize_rand(App->Player.angle - hfov_in_rad) + pixel_in_rad * pixel);
        glm::fvec2 point_end(App->Player.x + cos(angle), App->Player.y + -sin(angle));

        DDA_ray_collision ray_collision = DDA(App->map_tile, App->map_height, App->map_cols, point_end, point_start);

        if (ray_collision.collision_point.x != 0 && ray_collision.collision_point.y != 0) {
            float distance = fix_eye_fish(point_start, ray_collision.collision_point, (App->Player.angle - angle));

            int line_h     = (App->map_height * draw_screen_h) / distance;
            int line_start = (draw_screen_h / 2 - line_h / 2) - pitch;
            int line_end   = line_start + line_h;

            double wall_hit;
            if (ray_collision.side == 1) {
                /* for some reason wall_high is divived by 2 LOL */
                wall_hit = double(int(ray_collision.collision_point.x) % (wall_high / 2)) / (wall_high / 2);
            } else {
                wall_hit = double(int(ray_collision.collision_point.y) % (wall_high / 2)) / (wall_high / 2);
            }

            draw_vertical_view(App->texture[ray_collision.grid_index_collision - 1], wall_hit, line_start, line_end,
                               pixel);
        }
    }
}

void draw_3d_view_flor_and_ceil(AppGame *App) {
    int center_h = App->screen_heigh / 2;
    int pitch    = App->Player.pitch_view;

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

void draw_imgui(AppGame *App) {
    bool show_demo = false;
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();

    ImGui::NewFrame();
    if (show_demo)
        ImGui::ShowDemoWindow();
    ImGui::Render();
}

void draw(AppGame *App) {
    draw_3d_view_flor_and_ceil(App);
    draw_3d_view_port(App);
    draw_aim(App);
    draw_imgui(App);
}

void handle_mouse_motion(AppGame *App, SDL_Event *event) {
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

void engine_SDL_OpenGL_load_textures(AppGame *App) {
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

// main
int main(int argc, char *args[]) {
    srand(time(nullptr));
    AppGame App = {1920, 1080, "Simple Wolfenstein Engine", 1, 0, {300, 300, 0, cos(PI2), -sin(PI2), 0, 70}, 24,
                   24,   32};
    for (int x = 0; x < App.map_cols; x++)
        for (int y = 0; y < App.map_rows; y++)
            App.map_tile[x][y] = map[x][y];

    engine_SDL_OpenGL_setup(&App);
    engine_SDL_OpenGL_load_textures(&App);

    while (App.run_status) {
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            ImGui_ImplSDL2_ProcessEvent(&event);
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
        update(&App);

        /* draw here */
        draw(&App);

        /* update screen */
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(sdl_window);
    }

    /* shutdown */

    engine_SDL_OpenGL_shutdown();
    return 0;
}
