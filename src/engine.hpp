#include "engine.cpp"

#define PI 3.141592
#define PI2 6.283185
#define ONE_RAD 0.0174533

typedef struct {
        int a, w, s, d, shift, ctrl, j, k;
} ButtonKeys;

typedef struct {
        float x, y, xref, yref, button_r, button_l;
} Mouse;

typedef struct {
        float x, y;
        double angle, direction_x, direction_y;
        int pitch_view, fov;
        ButtonKeys Buttons;
} PLAYER;

typedef struct {
        int screen_width;
        int screen_heigh;
        std::string window_name;
        int run_status;
        int window_fullcreen;
        PLAYER Player;

        /* move to map */
        int map_cols;
        int map_rows;
        int map_height;
        int map_tile[32][32];

        /* engine */
        unsigned int *texture;
} AppGame;
