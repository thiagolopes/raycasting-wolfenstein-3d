#pragma once
#include <math.h>
#include "math.h"

#define SHADOW_MAX 25
#define NEAR 0.1
#define FAR 0.70

typedef struct {
    CardinalDir side;
    // what direction to step in x or y-direction (either +1 or -1)
    Point2i step;
    // which box of the map ray are in
    Point2i map_grid;
    // length of ray from current position to next x or y-side
    Point2h side_dist;
    // length of ray from one x or y-side to next x or y-side
    Point2h delta_dist;
} Ray;

// calculate step and initial side
// set steps base on direction growth on the grid;
// setup Ray->step and Ray->side_dist
Ray ray_setup(Point2h pos, Point2h ray_dir);
// walk ray to the next x and y map grid;
void ray_next_step(Ray* ray);
double ray_get_dist(Ray* ray);

double height_shadow(double ray_dist);
