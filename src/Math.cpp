#include <stdlib.h>

typedef struct Point {
    float x, y, z;
} Point;

typedef struct Color {
    uint8 r, g, b;
} Color;

typedef struct Sphere {
    Point center;
    float radius;
    Color color;
} Sphere;

typedef struct Scene {
    int size;
    Sphere spheres[];
} Scene;