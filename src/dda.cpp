#include <glm/fwd.hpp>
#include <glm/glm.hpp>

// structs
typedef enum { VERTICAL, HORIZONTAL } side_t;

typedef struct DDA_init {
    int (*map_grid)[];
    glm::fvec2 point_direction;
    glm::fvec2 point_start;
    float      dda_walk_max = 1000.0;
} DDA_init;

typedef struct {
    glm::fvec2 collision_point;      // ponto da collision
    int        grid_index_collision; // index do grid da collision
    int        side;                 // qual lado do grid foi a collision
} DDA_ray_collision;

// declarations
bool              check_map_bound_index(int index_x, int index_y, int index_max_x, int index_max_y);
DDA_ray_collision DDA(int map_tile[24][24], int map_height, int map_len, glm::fvec2 point_direction,
                      glm::fvec2 point_start);
// main
DDA_ray_collision DDA(int map_tile[24][24], int map_height, int map_len, glm::fvec2 point_direction,
                      glm::fvec2 point_start) {
    DDA_ray_collision dda_return;
    float             ray_total_max = 1000.0;
    float             ray_total     = 0.0;
    bool              ray_bound     = false;

    glm::fvec2 delta(point_direction.x / map_height - point_start.x / map_height,
                     point_direction.y / map_height - point_start.y / map_height);
    glm::fvec2 unitary_vector(delta.x / glm::length(delta), delta.y / glm::length(delta));
    glm::fvec2 unitary_step_size(sqrtf(1 + powf(unitary_vector.y / unitary_vector.x, 2)),
                                 sqrtf(1 + powf(unitary_vector.x / unitary_vector.y, 2)));
    glm::fvec2 ray_length_1D, step;
    glm::ivec2 tile_map_check(point_start.x, point_start.y);
    glm::ivec2 absolute_map_tile;

    /* set x walk */
    if (unitary_vector.x < 0) {
        step.x          = -1;
        ray_length_1D.x = (point_start.x - tile_map_check.x) * unitary_step_size.x;
    } else {
        step.x          = 1;
        ray_length_1D.x = ((tile_map_check.x + 1) - point_start.x) * unitary_step_size.x;
    }

    /* set y walk */
    if (unitary_vector.y < 0) {
        step.y          = -1;
        ray_length_1D.y = (point_start.y - tile_map_check.y) * unitary_step_size.y;
    } else {
        step.y          = 1;
        ray_length_1D.y = ((tile_map_check.y + 1) - point_start.y) * unitary_step_size.y;
    }

    while (!ray_bound && ray_total < ray_total_max) {
        /* walk */
        if (ray_length_1D.x < ray_length_1D.y) {
            tile_map_check.x += step.x;
            ray_total = ray_length_1D.x;
            ray_length_1D.x += unitary_step_size.x;
            dda_return.side = 0; /* vertical */
        } else {
            tile_map_check.y += step.y;
            ray_total = ray_length_1D.y;
            ray_length_1D.y += unitary_step_size.y;
            dda_return.side = 1; /* horizontal */
        }

        /* check */
        absolute_map_tile.x = tile_map_check.x / map_height;
        absolute_map_tile.y = tile_map_check.y / map_height;
        if (check_map_bound_index(absolute_map_tile.x, absolute_map_tile.y, map_len, map_len) == true
            && map_tile[absolute_map_tile.y][absolute_map_tile.x] != 0) {
            ray_bound                       = true;
            dda_return.grid_index_collision = map_tile[absolute_map_tile.y][absolute_map_tile.x];
        }
    }

    if (ray_bound == true) {
        dda_return.collision_point.x = point_start.x + unitary_vector.x * ray_total;
        dda_return.collision_point.y = point_start.y + unitary_vector.y * ray_total;
    }
    return dda_return;
}

bool check_map_bound_index(int index_x, int index_y, int index_max_x, int index_max_y) {
    if (index_x >= 0 && index_x < index_max_x && index_y >= 0 && index_y < index_max_y)
        return true;
    else
        return false;
}

//           ────────────────────────────────x─────────────────────────────────
//                          xx  P1        P2 x
//        (P1)The calculated xx              x   To fix the fish eye, just
// Ray has distortions based   xx            x     calculate perpendicular ray (P2)
// with the center, due to      xx           x     P1 * cos(angle)
// ray "casting" the screen.     xx          x
//                                xx         x
//                                 xx        x
//         The distortion cause     xx       x
//         the Fish Eye illusion     xx      x
//                                    xx     x
//                                     xx    x
//                                      xx   x
//                                       xx  x
//                                        xx x
//                                          xx
//                                        ┌──x──┐
//                                        │     │
//                                        │     │
//                                        └─────┘
float fix_eye_fish(glm::fvec2 point_start, glm::fvec2 collision_point, float angle) {
    return glm::length(point_start - collision_point) * cosf(angle);
}
