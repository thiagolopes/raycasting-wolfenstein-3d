#include <math.h>
#include "math.h"

Point2i point2i_normalize(Point2i point){
    int len = point2i_len(point);
    Point2i p = {(int)point.x / len, (int)point.y / len};
    return p;
}

Point2f point2f_normalize(Point2f point) {
    float len = point2f_len(point);
    Point2f p = {point.x / len, point.y / len};
    return p;
}

Point2i point2i_sub(Point2i a, Point2i b){
    Point2i p = {a.x - b.x, a.y - b.y};
    return p;
}

Point2f point2f_sub(Point2f a, Point2f b) {
    Point2f p = {a.x - b.x, a.y - b.y};
    return p;
}

Point2h point2h_sub(Point2h a, Point2h b){
    Point2h p = {a.x - b.x, a.y - b.y};
    return p;
}

int point2i_len(Point2i point){
    return sqrt(point.x * point.x + point.y * point.y);
}

float point2f_len(Point2f point) {
    return sqrtf(point.x * point.x + point.y * point.y);
}

float to_radf(float degree) {
    return degree * ONE_RAD;
}

