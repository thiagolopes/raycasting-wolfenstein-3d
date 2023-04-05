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
  int a, w, s, d;
} ButtonKeys;

typedef struct {
  float x, y, view_x, view_y, angle;
  ButtonKeys Buttons;
} Player;

typedef struct {
  float x, y, dist;
} Ray;

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
    1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0,
    0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0,
    1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
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
  SDL_Log("angle=%f, angle degree=%i ,x=%f, y=%f, viewx=%f ,viewy=%f",
          App->Player.angle, rad_to_degree(App->Player.angle), App->Player.x,
          App->Player.y, App->Player.view_x, App->Player.view_y);
}

void ray_horizontal(float player_x, float player_y, float angle,
                    int collision_point[]) {
  int px = 0, py = 0, dx, dy, n = block_in_size;
  float tan_angle = tan(angle);

  if (sin(angle) > 0.0001) {
    /* up */
    py = ((int)(player_y / n) * n - 1);
    dy = -64;
  }
  if (sin(angle) < -0.0001) {
    /* down */
    py = ((int)(player_y / n) * n + n);
    dy = 64;
  }
  px = (int)player_x + (int)((player_y - py) / tan_angle);
  dx = (int)(n / tan_angle);

  int collision = mapX;
  while (collision) {
    int x = (int)px / n;
    int y = (int)py / n;
    int map_index = x_y_to_linear_2d_map(x, y);

    if (map_index > 0 && map_index < mapX * mapY && map[map_index] != 0) {
      SDL_Log("HORIZONTAL COLLISION index= %i, map[]=%i, px_i=%i, px_i=%i, "
              "collision_steps=%i",
              map_index, map[map_index], (int)px / n, (int)py / n,
              mapY - collision);
      collision = 0;
    } else {
      px += dx;
      py += dy;
      collision--;
    }
  }

  collision_point[0] = px;
  collision_point[1] = py;
}

void ray_vertical(float player_x, float player_y, float angle,
                  int collision_point[]) {
  int px = 0, py = 0, dx, dy, n = block_in_size;
  float tan_angle = tan(angle);

  if (cos(angle) > 0.0001) {
    /* right */
    SDL_Log("right");
    px = ((int)(player_x / n) * n + n);
    dx = 64;
  }
  if (cos(angle) < -0.0001) {
    /* left */
    SDL_Log("left");
    px = ((int)(player_x / n) * n - 1);
    dx = -64;
  }
  py = (int)player_y + (int)((player_x - px) * tan_angle);
  dy = (int)(n * tan_angle);

  int collision = mapX;
  while (collision) {
    int x = (int)px / n;
    int y = (int)py / n;
    int map_index = x_y_to_linear_2d_map(x, y);

    if (map_index > 0 && map_index < mapX * mapY && map[map_index] != 0) {
      SDL_Log("VERTICAL COLLISION index= %i, map[]=%i, px_i=%i, px_i=%i, "
              "collision_steps=%i",
              map_index, map[map_index], (int)px / n, (int)py / n,
              mapY - collision);
      collision = 0;
    } else {
      px += dx;
      py += dy;
      collision--;
    }
  }

  collision_point[0] = px;
  collision_point[1] = py;
}

float dist(float ax, float ay, float bx, float by) {
  return sqrt((bx - ax) * (bx - ax) + (by - ay) * (by - ay));
}

void draw_ray(AppGame *App) {
  int ray_point_horizontal[2];
  int ray_point_vertical[2];
  ray_horizontal(App->Player.x, App->Player.y, App->Player.angle,
                 ray_point_horizontal);
  ray_vertical(App->Player.x, App->Player.y, App->Player.angle,
               ray_point_vertical);

  glLineWidth(4);
  glBegin(GL_LINES);
  glColor3f(0, 1, 0);
  glVertex2i(App->Player.x, App->Player.y);
  glVertex2i(ray_point_horizontal[0], ray_point_horizontal[1]);
  glColor3f(1, 0, 0);
  glVertex2i(App->Player.x, App->Player.y);
  glVertex2i(ray_point_vertical[0], ray_point_vertical[1]);
  glEnd();

  /* float dist1 = dist(App->Player.x, App->Player.y, ray_point_horizontal[0],
   * ray_point_horizontal[1]); */
  /* float dist2 = dist(App->Player.x, App->Player.y, ray_point_vertical[0],
   * ray_point_vertical[1]); */

  /* glLineWidth(3); */
  /* glBegin(GL_LINES); */

  /* glVertex2i(App->Player.x, App->Player.y); */
  /* if (dist1 < dist2){ */
  /*   glColor3f(0, 1,0); */
  /*   SDL_Log("HORIZONTAL!!!!!!!!!!!!!!!!!!!!"); */
  /*   glVertex2i(ray_point_horizontal[0], ray_point_horizontal[1]); */
  /* }else{ */
  /*   glColor3f(1, 0, 0); */
  /*   SDL_Log("VERTICAL!!!!!!!!!!!!!!!!!!!!!!"); */
  /*   glVertex2i(ray_point_vertical[0], ray_point_vertical[1]); */
  /* } */
  /* glEnd(); */
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
  glVertex2i(App->Player.x + App->Player.view_x * 20,
             App->Player.y + App->Player.view_y * 20);
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

static void handle_key(SDL_Keysym keysym, AppGame *App, int flag) {
  switch (keysym.sym) {
  case SDLK_ESCAPE:
    App->run_status = 0;
    break;
  case SDLK_a:
    App->Player.Buttons.a = flag;
    break;
  case SDLK_d:
    App->Player.Buttons.d = flag;
    break;
  case SDLK_w:
    App->Player.Buttons.w = flag;
    break;
  case SDLK_s:
    App->Player.Buttons.s = flag;
    break;
  }

  SDL_Log("a=%i, d%i, w=%i, s=%i", App->Player.Buttons.a, App->Player.Buttons.d,
          App->Player.Buttons.w, App->Player.Buttons.s);
}

int main(int argc, char *args[]) {
  /* init */
  AppGame App = {1024, 512, "Project raycasting",
                 1,    0,   {300, 300, cos(PI2), -sin(PI2), PI2}};

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVERYTHING | SDL_INIT_NOPARACHUTE) <
      0) {
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
  SDL_VideoQuit();
  SDL_GL_DeleteContext(sdl_gl_context);
  SDL_DestroyWindow(sdl_window);
  SDL_Quit();
  return 0;
}
