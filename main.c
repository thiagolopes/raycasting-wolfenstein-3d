#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_timer.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define PI 3.14
#define PI2 (PI * 2)
#define ONE_RAD PI / 180

void drawCircle(float cx, float cy, float r, int num_segments)
{
  float theta = 3.1415926 * 2 / num_segments;
  float tangetial_factor = tanf(theta);
  float radial_factor = cosf(theta); // calculate the radial factor
  float x = r;                       // we start at angle = 0

  float y = 0;
  glLineWidth(2);
  glBegin(GL_LINE_LOOP);
  for(int ii = 0; ii < num_segments; ii++)
    {
      glVertex2f(x + cx, y + cy); // output vertex

      // calculate the tangential vector
      // remember, the radial vector is (x, y)
      // to get the tangential vector we flip those coordinates and negate one
      // of them

      float tx = -y;
      float ty = x;

      // add the tangential vector

      x += tx * tangetial_factor;
      y += ty * tangetial_factor;

      // correct using the radial factor

      x *= radial_factor;
      y *= radial_factor;
    }
  glEnd();
}

typedef struct
{
  float x, y;
} Pointf;

typedef struct
{
  int x, y;
} Point;

Pointf MOUSE_POINT;
int MOUSE_POINT_SHOW;
Pointf INTERSECTION_POINT;

typedef struct
{
  Point p1, p2;
  float length;
} Vector;

typedef struct
{
  int a, w, s, d;
} ButtonKeys;

typedef struct
{
  float x, y, direction_x, direction_y, angle;
  ButtonKeys Buttons;
} Player;

typedef struct
{
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

} AppGame;

SDL_Window *sdl_window = NULL;
SDL_GLContext sdl_gl_context;

float normalizedRand(float rand)
{
  if(rand > PI2)
    return rand - PI2;
  if(rand < 0)
    return rand + PI2;
  return rand;
}

void updatePlayer(AppGame *App)
{
  int magntude = 2;
  if(App->Player.Buttons.d == 1)
    {
      App->Player.angle -= ONE_RAD;
      App->Player.direction_x = cos(App->Player.angle);
      App->Player.direction_y = -sin(App->Player.angle);
    }
  if(App->Player.Buttons.a == 1)
    {
      App->Player.angle += ONE_RAD;
      App->Player.direction_x = cos(App->Player.angle);
      App->Player.direction_y = -sin(App->Player.angle);
    }
  if(App->Player.Buttons.w == 1)
    {
      App->Player.x += App->Player.direction_x * magntude;
      App->Player.y += App->Player.direction_y * magntude;
    }
  if(App->Player.Buttons.s == 1)
    {
      App->Player.x -= App->Player.direction_x * magntude;
      App->Player.y -= App->Player.direction_y * magntude;
    }

  App->Player.angle = normalizedRand(App->Player.angle);
}

float dist(float ax, float ay, float bx, float by)
{
  return sqrt((bx - ax) * (bx - ax) + (by - ay) * (by - ay));
}

void draw_line(float x1, float y1, float x2, float y2)
{
  glBegin(GL_LINES);
  glColor3f(.7, .9, .3);
  glLineWidth(.1);
  glVertex2i(x1, y1);
  glVertex2i(x2, y2);
  glEnd();
}

void draw_player(AppGame *App)
{
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
  glVertex2i(App->Player.x + App->Player.direction_x * 20,
             App->Player.y + App->Player.direction_y * 20);
  glEnd();
}

void draw_map_2d(AppGame *App)
{
  float pixel_x, pixel_y;
  for(int l = 0; l < App->map_rows; l++)
    {
      for(int c = 0; c < App->map_cols; c++)
        {
          if(App->map_tile[l][c] != 0)
            {
              pixel_x = l * App->map_height;
              pixel_y = c * App->map_height;

              glBegin(GL_QUADS);
              glColor3f(1, 1, 1);
              glVertex2f(pixel_x, pixel_y);
              glVertex2f(pixel_x, pixel_y + App->map_height);
              glVertex2f(pixel_x + App->map_height, pixel_y + App->map_height);
              glVertex2f(pixel_x + App->map_height, pixel_y);
              glEnd();
            }
        }
    }
}

void draw_mouse_pointer(AppGame *App)
{
  if(MOUSE_POINT_SHOW == 1)
    {
      /* TODO move to draw dashed */
      glPushAttrib(GL_ENABLE_BIT);
      /* # glPushAttrib is done to return everything to normal after drawing */
      glColor3f(0, 1, 0);
      glLineStipple(8, 0xAAAA);
      glEnable(GL_LINE_STIPPLE);
      glBegin(GL_LINES);
      glVertex2i(App->Player.x, App->Player.y);
      glVertex2i(MOUSE_POINT.x, MOUSE_POINT.y);
      glEnd();
      glPopAttrib();

      drawCircle(MOUSE_POINT.x, MOUSE_POINT.y, 10, 10);
      /* glColor3f(0, 1, 0); */
      /* glLineWidth(2); */
      /* glBegin(GL_LINES); */
      /* glVertex2i(App->Player.x, App->Player.y); */
      /* glVertex2i(MOUSE_POINT.x, MOUSE_POINT.y); */
      /* drawCircle(MOUSE_POINT.x, MOUSE_POINT.y, 10, 10); */
      /* glEnd(); */
    }

  if(INTERSECTION_POINT.x != 0 && INTERSECTION_POINT.y != 0)
    {
      glBegin(GL_POINTS);
      glColor3f(1, 0, 0);
      glPointSize(8);
      glVertex2i(INTERSECTION_POINT.x, INTERSECTION_POINT.y);
      glEnd();
    }
}

void draw(AppGame *App)
{
  draw_player(App);
  draw_map_2d(App);
  draw_mouse_pointer(App);
}

static void handle_key(SDL_Keysym keysym, AppGame *App, int button_action)
{
  switch(keysym.sym)
    {
    case SDLK_ESCAPE: App->run_status = 0; break;
    case SDLK_a: App->Player.Buttons.a = button_action; break;
    case SDLK_d: App->Player.Buttons.d = button_action; break;
    case SDLK_w: App->Player.Buttons.w = button_action; break;
    case SDLK_s: App->Player.Buttons.s = button_action; break;
    }

  SDL_Log("a=%i, d%i, w=%i, s=%i", App->Player.Buttons.a,
          App->Player.Buttons.d, App->Player.Buttons.w, App->Player.Buttons.s);
}

void SDLOpenGLSetup(AppGame App)
{
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVERYTHING | SDL_INIT_NOPARACHUTE) < 0)
    {
      perror(SDL_GetError());
    }
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                      SDL_GL_CONTEXT_PROFILE_CORE);

  sdl_window = SDL_CreateWindow(
    App.window_name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    App.screen_width, App.screen_heigh, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  if(sdl_window == NULL)
    {
      perror(SDL_GetError());
    }

  sdl_gl_context = SDL_GL_CreateContext(sdl_window);
  if(sdl_gl_context == NULL)
    {
      perror(SDL_GetError());
    }

  /* setup vsync */
  if(SDL_GL_SetSwapInterval(1) < 0)
    {
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

  if(App.window_fullcreen)
    SDL_SetWindowFullscreen(sdl_window, SDL_WINDOW_FULLSCREEN);
}

void draw_dot(float x, float y)
{
  glColor3f(1, 1, 1);
  glPointSize(1);
  glBegin(GL_POINTS);
  glVertex2i(x, y);
  glEnd();
}

void DDA_Algorith(AppGame *App, Pointf *point_collision, Pointf *point_direction)
{
  int bound = 0;
  float ray_dist = 0;
  float ray_dist_max_walk = 1000.0;

  Pointf point_start = {App->Player.x, App->Player.y},
         delta
         = {point_direction->x / App->map_height - point_start.x / App->map_height,
            point_direction->y / App->map_height - point_start.y / App->map_height};

  float point_magnitude = sqrtf(powf(delta.x, 2) + powf(delta.y, 2));

  Pointf unitary_vector
    = {delta.x / point_magnitude, delta.y / point_magnitude},
    unitary_step_size
    = {sqrtf(1 + powf(unitary_vector.y / unitary_vector.x, 2)),
       sqrtf(1 + powf(unitary_vector.x / unitary_vector.y, 2))},
    ray_length_1D, step;

  Point tile_map_check = {truncf(point_start.x) , truncf(point_start.y)};

  /* set x walk */
  if(unitary_vector.x < 0)
    {
      step.x = -1;
      ray_length_1D.x
        = (point_start.x - (float)tile_map_check.x) * unitary_step_size.x;
    }
  else
    {
      step.x = 1;
      ray_length_1D.x = (((float)tile_map_check.x + 1) - point_start.x)
                        * unitary_step_size.x;
    }

  /* set y walk */
  if(unitary_vector.y < 0)
    {
      step.y = -1;
      ray_length_1D.y
        = (point_start.y - (float)tile_map_check.y) * unitary_step_size.y;
    }
  else
    {
      step.y = 1;
      ray_length_1D.y = (((float)tile_map_check.y + 1) - point_start.y)
                        * unitary_step_size.y;
    }

  while(!bound && ray_dist < ray_dist_max_walk)
    {
      /* walk */
      if(ray_length_1D.x < ray_length_1D.y)
        {
          tile_map_check.x += step.x;
          ray_dist = ray_length_1D.x;
          ray_length_1D.x += unitary_step_size.x;
        }
      else
        {
          tile_map_check.y += step.y;
          ray_dist = ray_length_1D.y;
          ray_length_1D.y += unitary_step_size.y;
        }

      /* check */
      Point absolute_map_tile = {tile_map_check.x / App->map_height,
                                 tile_map_check.y / App->map_height};
      if(absolute_map_tile.x >= 0 && absolute_map_tile.x < App->map_cols
         && absolute_map_tile.y >= 0 && absolute_map_tile.y < App->map_rows)
        {
          if(App->map_tile[absolute_map_tile.x][absolute_map_tile.y] == 1)
            {
              bound = 1;
            }
        }
    }

  if(bound == 1)
    {
      point_collision->x = tile_map_check.x;
      point_collision->y = tile_map_check.y;
    }
}

void SDLOpenGLShutdown()
{
  SDL_VideoQuit();
  SDL_GL_DeleteContext(sdl_gl_context);
  SDL_DestroyWindow(sdl_window);
  SDL_Quit();
}

#define INT(x) ((int)x)
void handle_mouse_pressed_down(int button, float x, float y, AppGame *App)
{
  switch(button)
    {
    case SDL_BUTTON_LEFT:
      App->map_tile[INT(truncf(x)) / App->map_height]
                   [INT(truncf(y)) / App->map_height]
        = 1;
      break;
    case SDL_BUTTON_RIGHT:
      MOUSE_POINT_SHOW = 1;
      MOUSE_POINT.x = x;
      MOUSE_POINT.y = y;
      break;
    }
}

void handle_mouse_pressed_up(int button, float x, float y, AppGame *App)
{
  switch(button)
    {
    case SDL_BUTTON_LEFT: break;
    case SDL_BUTTON_RIGHT: MOUSE_POINT_SHOW = 0; break;
    }
}

int main(int argc, char *args[])
{
  /* init */
  srand(time(NULL));
  AppGame App = {1024, 1024, "Project raycasting",
                 1,    0,    {300, 300, cos(PI2), -sin(PI2), PI2}};
  App.map_cols = 32;
  App.map_rows = 32;
  App.map_height = 32;

  SDLOpenGLSetup(App);

  int start_time, frame_time;
  float fps;

  while(App.run_status)
    {
      start_time = SDL_GetTicks();

      SDL_Event event;
      while(SDL_PollEvent(&event) != 0)
        {
          switch(event.type)
            {
            case SDL_QUIT: App.run_status = 0; break;
            case SDL_KEYDOWN: handle_key(event.key.keysym, &App, 1); break;
            case SDL_KEYUP: handle_key(event.key.keysym, &App, 0); break;
            case SDL_MOUSEMOTION:
              if(event.motion.state == (SDL_BUTTON_LMASK | SDL_BUTTON_RMASK))
                {
                  handle_mouse_pressed_down(SDL_BUTTON_LEFT, event.button.x,
                                            event.button.y, &App);
                  handle_mouse_pressed_down(SDL_BUTTON_RIGHT, event.button.x,
                                            event.button.y, &App);
                }
              else if(event.motion.state == SDL_BUTTON_LMASK)
                {
                  handle_mouse_pressed_down(SDL_BUTTON_LEFT, event.button.x,
                                            event.button.y, &App);
                }
              else if(event.motion.state == SDL_BUTTON_RMASK)
                {
                  handle_mouse_pressed_down(SDL_BUTTON_RIGHT, event.button.x,
                                            event.button.y, &App);
                }
              break;
            case SDL_MOUSEBUTTONDOWN:
              handle_mouse_pressed_down(event.button.button, event.button.x,
                                        event.button.y, &App);
              break;
            case SDL_MOUSEBUTTONUP:
              handle_mouse_pressed_up(event.button.button, event.button.x,
                                      event.button.y, &App);
              break;
            }
        }

      /* clear screen */
      glClear(GL_COLOR_BUFFER_BIT);

      /* update here */
      updatePlayer(&App);

      /* draw here */
      draw(&App);

      /* draw rayview */
      int fov = 70;
      for(int i = -(fov / 2); i < fov / 2; i++)
        {
          float angle = normalizedRand(App.Player.angle + (ONE_RAD * i));
          Pointf point_end = {App.Player.x + cos(angle),
                              App.Player.y + -sin(angle)};
          Pointf wall = {0, 0};
          DDA_Algorith(&App, &wall, &point_end);
          if (wall.x != 0 && wall.y != 0)
            draw_line(App.Player.x, App.Player.y, wall.x, wall.y);
        };

      /* draw dda Algorith */
      Pointf collision;
      if(MOUSE_POINT_SHOW == 1)
        {
          DDA_Algorith(&App, &collision, &MOUSE_POINT);
          drawCircle(collision.x, collision.y, 10, 10);
        }

      /* update screen */
      SDL_GL_SwapWindow(sdl_window);

      /* fps print */
      frame_time = SDL_GetTicks() - start_time;
      if(frame_time > 0)
        fps = 1000.0 / frame_time;
      else
        fps = 0;

      SDL_Log("FPS: %f", fps);
    }

  /* shutdown */
  SDLOpenGLShutdown();
  return 0;
}
