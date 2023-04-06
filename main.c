#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_opengl.h>
#include <stdio.h>
#include <stdlib.h>

#define PI 3.14
#define PI2 (PI * 2)
#define ONE_RAD PI / 180

typedef struct {
  float x, y;
} Point;

typedef struct {
  int a, w, s, d;
} ButtonKeys;

typedef struct {
  float x, y, view_x, view_y, angle;
  ButtonKeys Buttons;
} Player;

typedef struct {
  int screen_width;
  int screen_heigh;
  char *window_name;
  int run_status;
  int window_fullcreen;
  Player Player;
} AppGame;

SDL_Window *sdl_window = NULL;
SDL_GLContext sdl_gl_context;

/* map  */
int mapX = 8, mapY = 8, block_in_size = 64;
int map[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1,
    0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0,
    1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

int x_y_to_linear_2d_map(int x, int y) { return y * mapX + x; }

int rad_to_degree(float angle) { return (int)(angle * 180 / PI); }

float normalizedRand(float rand) {
  if (rand > PI2)
    return rand - PI2;
  if (rand < 0)
    return rand + PI2;
  return rand;
}

void updatePlayer(AppGame *App) {
  int magntude = 2;
  if (App->Player.Buttons.d == 1) {
    App->Player.angle -= ONE_RAD;
    App->Player.view_x = cos(App->Player.angle);
    App->Player.view_y = -sin(App->Player.angle);
  }
  if (App->Player.Buttons.a == 1) {
    App->Player.angle += ONE_RAD;
    App->Player.view_x = cos(App->Player.angle);
    App->Player.view_y = -sin(App->Player.angle);
  }
  if (App->Player.Buttons.w == 1) {
    App->Player.x += App->Player.view_x * magntude;
    App->Player.y += App->Player.view_y * magntude;
  }
  if (App->Player.Buttons.s == 1) {
    App->Player.x -= App->Player.view_x * magntude;
    App->Player.y -= App->Player.view_y * magntude;
  }

  App->Player.angle = normalizedRand(App->Player.angle);
}

/* TODO update to point */
/* TODO move to ray lib */
void ray_horizontal(float player_x, float player_y, float angle, Point *rnt_point) {
  float px = 0, py = 0, dx, dy;
  int n = block_in_size;
  float tan_angle = 1 / tan(angle);

  if (angle < PI) {
    /* up */
    py = ((int)(player_y / n) * n) - 1;
    px = player_x + ((player_y - py) * tan_angle);
    dy = -n;
  } else if (angle > PI) {
    /* down */
    py = ((int)(player_y / n) * n) + n;
    px = player_x - ((py - player_y) * tan_angle);
    dy = n;
  }
  dx = -dy * tan_angle;

  int collision = mapX;
  while (collision) {
    int x = (int)px / n;
    int y = (int)py / n;
    int map_index = x_y_to_linear_2d_map(x, y);

    if (map_index > 0 && map_index < mapX * mapY && map[map_index] == 1) {
      collision = 0;
    } else {
      px += dx;
      py += dy;
      collision -= 1;
    }
    rnt_point->x = px;
    rnt_point->y = py;
  }
}

/* update to return a point instead a void */
void ray_vertical(float player_x, float player_y, float angle, Point *rnt_point) {
  float px = 0, py = 0, dx, dy;
  int n = block_in_size;
  float tan_angle = -1 / tan(angle);

  if (angle < PI / 2 || angle > (PI * 3) / 2) {
    /* right */
    px = ((int)(player_x / n) * n) + n;
    py = player_y - ((player_x - px) / tan_angle);
    dx = n;
  } else if (angle > PI / 2 && angle < (PI * 3) / 2) {
    /* left */
    px = ((int)(player_x / n) * n) - 1;
    py = player_y + ((px - player_x) / tan_angle);
    dx = -n;
  }
  dy = dx / tan_angle;

  int collision = mapX;
  while (collision) {
    int x = (int)px / n;
    int y = (int)py / n;
    int map_index = x_y_to_linear_2d_map(x, y);

    if (map_index > 0 && map_index < mapX * mapY && map[map_index] == 1) {
      collision = 0;
    } else {
      px += dx;
      py += dy;
      collision -= 1;
    }
    rnt_point->x = px;
    rnt_point->y = py;
  }
}

float dist(float ax, float ay, float bx, float by) {
  return sqrt((bx - ax) * (bx - ax) + (by - ay) * (by - ay));
}

void draw_ray(AppGame *App) {
  Point point_horizontal, point_vertical;
  ray_horizontal(App->Player.x, App->Player.y, App->Player.angle, &point_horizontal);
  ray_vertical(App->Player.x, App->Player.y, App->Player.angle, &point_vertical);

  float dist1 = dist(App->Player.x, App->Player.y, point_horizontal.x, point_horizontal.y);
  float dist2 = dist(App->Player.x, App->Player.y, point_vertical.x, point_vertical.y);

  glLineWidth(3);
  glBegin(GL_LINES);
  glVertex2i(App->Player.x, App->Player.y);
  glColor3f(1, 0, 0);
  if (fabsf(dist1) < fabsf(dist2)) {
    glVertex2i(point_horizontal.x, point_horizontal.y);
  } else {
    glVertex2i(point_vertical.x, point_vertical.y);
  }
  glEnd();
}

void drawPlayer(AppGame *App) {
  glColor3f(0, 0, 1);
  glPointSize(8);
  glBegin(GL_POINTS);
  glVertex2i(App->Player.x, App->Player.y);
  glEnd();

  /* draw direction */
  glColor3f(.5, .5, .5);
  glLineWidth(3);
  glBegin(GL_LINES);
  glVertex2i(App->Player.x, App->Player.y);
  glVertex2i(App->Player.x + App->Player.view_x * 20, App->Player.y + App->Player.view_y * 20);
  glEnd();
}

void draw2DMap() {
  int xo, yo;
  int grid_lenth = 512 / mapX;
  for (int y = 0; y < mapY; y++) {
    for (int x = 0; x < mapX; x++) {
      int xo = x * grid_lenth;
      int yo = y * grid_lenth;

      if (map[y * mapX + x]) {
        glColor3f(1, 1, 1);
      } else {
        glColor3f(0, 0, 0);
      }
      glBegin(GL_QUADS);
      glVertex2i(xo + 1, yo + 1);
      glVertex2i(xo + 1, yo + grid_lenth - 1);
      glVertex2i(xo + grid_lenth - 1, yo + grid_lenth - 1);
      glVertex2i(xo + grid_lenth - 1, yo + 1);
      glEnd();
    }
  }
}

void draw(AppGame *App) {
  draw2DMap();
  drawPlayer(App);
  draw_ray(App);
}

static void handle_key(SDL_Keysym keysym, AppGame *App, int button_action
) {
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
  }

  SDL_Log("a=%i, d%i, w=%i, s=%i", App->Player.Buttons.a, App->Player.Buttons.d, App->Player.Buttons.w, App->Player.Buttons.s);
}

void SDLOpenGLSetup(AppGame App){
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

  sdl_window = SDL_CreateWindow(
      App.window_name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
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
  glEnable(GL_TEXTURE_2D);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  if (App.window_fullcreen)
    SDL_SetWindowFullscreen(sdl_window, SDL_WINDOW_FULLSCREEN);

}

void SDLOpenGLShutdown(){
  SDL_VideoQuit();
  SDL_GL_DeleteContext(sdl_gl_context);
  SDL_DestroyWindow(sdl_window);
  SDL_Quit();
}

int main(int argc, char *args[]) {
  /* init */
  AppGame App = {1024, 512, "Project raycasting", 1,    0,   {300, 300, cos(PI2), -sin(PI2), PI2}};

  SDLOpenGLSetup(App);

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
      }
    }

    /* clear screen */
    glClear(GL_COLOR_BUFFER_BIT);

    /* update here */
    updatePlayer(&App);

    /* draw here */
    draw(&App);

    /* update screen */
    SDL_GL_SwapWindow(sdl_window);
  }

  /* shutdown */
  SDLOpenGLShutdown();
  return 0;
}
