#define PI 3.141592
#define TAU 6.283185
#define ONE_RAD 0.0174533

typedef struct{
    float x;
    float y;
    float width;
    float height;
} Rectanglef;

typedef struct Point2f {
    float x;
    float y;
} Point2f;

typedef struct Point2i {
    int x;
    int y;
} Point2i;

Point2i point2i_normalize(Point2i point);
Point2i point2i_sub(Point2i a, Point2i b);
int point2i_len(Point2i point);

Point2f point2f_normalize(Point2f point);
Point2f point2f_sub(Point2f a, Point2f b);
float point2f_len(Point2f point);

float degree_to_rad(float degree);
