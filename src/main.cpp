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
#include <sstream>
#include <string>
#include <dirent.h>

// internal
#include "main.h"
#include "colors.h"
#include "dda.cpp"
#include "draw.cpp"

// imgui
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_sdl2.h"
#include "imgui/imgui.h"

// stb
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// methods
void update_player(AppGame *App) {
    float magntude = 2;
    if (App->Buttons.shift == 1)
        magntude = 4;

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
                                  App->screen_width, App->screen_height, SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
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
    glOrtho(0, App->screen_width, App->screen_height, 0, 1, -1);
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
        int y = (App->screen_height / 2) - 10;

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
        int y = (App->screen_height / 2);

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
    unsigned char *data = stbi_load(texture_name.c_str(), &f_width, &f_height, &bpp, STBI_rgb_alpha);
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

void draw_3d_view_port(AppGame *App) {
    int pixels_cols    = App->screen_width;
    int draw_screen_h  = App->screen_height;
    int wall_height    = App->map_height;
    int map_len        = App->map_cols;
    int(*map)[24]      = App->map_tile;
    int   pitch        = App->Player.pitch_view; /* player look up or down */
    float player_angle = App->Player.angle;
    float fov_in_rad   = App->Player.fov * ONE_RAD;
    float hfov_in_rad  = (App->Player.fov / 2) * ONE_RAD;
    float pixel_in_rad = fov_in_rad / pixels_cols;

    glm::fvec2 point_start = glm::fvec2(App->Player.x, App->Player.y);
    Color_t    draw_color;
    float      fade_draw_vertical = 1.5;

    for (int pixel = 0; pixel < pixels_cols; pixel++) {
        int        line_h;
        int        line_start;
        int        line_end;
        float      texture_offset;
        float      angle;
        float      distance;
        glm::fvec2 point_end;
        DDA_t      ray_collision;

        angle       = normalize_rand(normalize_rand(player_angle - hfov_in_rad) + pixel_in_rad * pixel);
        point_end.x = point_start.x + cos(angle);
        point_end.y = point_start.y + -sin(angle);

        ray_collision = DDA(map, wall_height, map_len, point_end, point_start);

        if (ray_collision.valid) {
            distance = fix_eye_fish(point_start - ray_collision.collision_point, player_angle - angle);

            line_h     = (wall_height * draw_screen_h) / distance;
            line_start = (draw_screen_h / 2 - line_h / 2) - pitch;
            line_end   = line_start + line_h;

            if (ray_collision.side == HORIZONTAL) {
                draw_color.r   = 255;
                draw_color.g   = 255;
                draw_color.b   = 255;
                texture_offset = float(int(ray_collision.collision_point.x) % wall_height) / wall_height;
            } else {
                draw_color.r   = 255 / fade_draw_vertical;
                draw_color.g   = 255 / fade_draw_vertical;
                draw_color.b   = 255 / fade_draw_vertical;
                texture_offset = float(int(ray_collision.collision_point.y) % wall_height) / wall_height;
            }

            draw_vertical_line(pixel, line_start, line_end, ray_collision.grid_index_collision, texture_offset,
                               draw_color);
        }
    }
}

void draw_3d_view_floor(AppGame *App) {
    float       floor_start = App->screen_height / 2 - App->Player.pitch_view;
    Rectangle_t floor{0, floor_start, (float)App->screen_width, (float)App->screen_height + App->Player.pitch_view};
    draw_rect(floor, DARKGRAY);
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
    draw_3d_view_floor(App);
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
    char texture_dir[100] = "./textures/";
    // setup texture memory
    App->texture_len = 0;

    DIR           *textures_dirs;
    struct dirent *ent;
    if ((textures_dirs = opendir(texture_dir)) != nullptr) {
        while ((ent = readdir(textures_dirs)) != nullptr) {
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
}

// main
int main(int argc, char *args[]) {
    srand(time(nullptr));
    std::string       title_format = "Simple Wolfenstein Engine - FPS %i";
    char              title[256];
    std::stringstream title_ss;
    AppGame           App = {1920, 1080, title, 1, 0, {300, 300, 0, cos(PI2), -sin(PI2), 0, 70}, 24, 24, 32};
    for (int x = 0; x < App.map_cols; x++)
        for (int y = 0; y < App.map_rows; y++)
            App.map_tile[x][y] = map[x][y];

    engine_SDL_OpenGL_setup(&App);
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
            std::sprintf(title, title_format.c_str(), frame_counter);
            SDL_SetWindowTitle(sdl_window, title);
            frame_counter = 0;
            frame_timer   = current_time;
        }

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

        if (delta > frame_delta) {
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

            last_time = current_time;
            ++frame_counter;
        }
    }

    /* shutdown */

    engine_SDL_OpenGL_shutdown();
    return 0;
}
