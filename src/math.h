#pragma once
#define PI 3.141592
#define TAU 6.283185
#define ONE_RAD 0.0174533

typedef struct {
    float x;
    float y;
} Point2f;

typedef struct {
    int x;
    int y;
} Point2i;

typedef struct {
    double x;
    double y;
} Point2h;

// four cardinal directions are the four main directions: north, south, east, and west
typedef enum {
    NS,
    WE,
} CardinalDir;

Point2i point2i_normalize(Point2i point);
Point2f point2f_normalize(Point2f point);
Point2h point2h_normalize(Point2h point);

Point2i point2i_sub(Point2i a, Point2i b);
Point2f point2f_sub(Point2f a, Point2f b);
Point2h point2h_sub(Point2h a, Point2h b);

int    point2i_len(Point2i point);
float  point2f_len(Point2f point);
double point2h_len(Point2i point);

float  to_degreef(float radian);
double to_degreeh(double radian);

float  to_radf(float degree);
double to_radh(double degree);

double normalize_numberf(double n, double len);
double normalize_numberh(double n, double len);

Point2h point2h_rotate(Point2h p, double angle);
