#include "algorithm.h"
#include <stdlib.h>

Ray ray_setup(Point2h pos, Point2h ray_dir) {
    Ray ray = {0};

    ray.map_grid   = (Point2i){(int)pos.x, (int)pos.y};
    ray.delta_dist = (Point2h){fabs(1 / ray_dir.x), fabs(1 / ray_dir.y)};

    // is left
    if (ray_dir.x < 0) {
        ray.step.x = -1;

        ray.side_dist.x = (pos.x - ray.map_grid.x) * ray.delta_dist.x;
    }
    // is right
    else {
        ray.step.x = 1;

        ray.side_dist.x = (ray.map_grid.x + 1.0 - pos.x) * ray.delta_dist.x;
    }

    if (ray_dir.y < 0) {
        ray.step.y = -1;

        ray.side_dist.y = (pos.y - ray.map_grid.y) * ray.delta_dist.y;
    } else {
        ray.step.y = 1;

        ray.side_dist.y = (ray.map_grid.y + 1.0 - pos.y) * ray.delta_dist.y;
    }

    return ray;
};

void ray_next_step(Ray* ray) {
    // jump to next map square, either in x-direction, or in y-direction
    if (ray->side_dist.x < ray->side_dist.y) {
        ray->side_dist.x += ray->delta_dist.x;
        ray->map_grid.x += ray->step.x;
        ray->side = NS;
    } else {
        ray->side_dist.y += ray->delta_dist.y;
        ray->map_grid.y += ray->step.y;
        ray->side = WE;
    }
};

inline double ray_get_dist(Ray* ray) {
    // last hit on x coordinatew
    if (ray->side == NS) {
        return point2h_sub(ray->side_dist, ray->delta_dist).x;
    }
    // last hit on y coordinatew
    else {
        return point2h_sub(ray->side_dist, ray->delta_dist).y;
    }
    exit(EXIT_FAILURE); // do not reach here;
};

double height_shadow(double ray_dist) {
    double s_force      = ray_dist / SHADOW_MAX;
    double s_normalized = exp(-s_force * s_force / (FAR * NEAR));
    return (double)255 * s_normalized;
}
