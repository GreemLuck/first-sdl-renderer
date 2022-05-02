#pragma once
#ifndef _RENDERMATH_H
#define _RENDERMATH_H

#include <stdlib.h>
#include <math.h>
#include <tuple>

typedef struct Point {
	double x, y, z;
} Point;

typedef struct Color {
	int r, g, b, a;
} Color;

typedef struct Vector {
	double x, y, z;
} Vector;

Vector NewVector(Point p);
Vector NewVector(Point p1, Point p2);
double GetNorm(Vector v); 
Vector Normalize(Vector v);
double Dot(Vector v1, Vector v2);

typedef struct Sphere {
	Point center;
	double radius;
	Color color;
} Sphere;

std::tuple<double, double> IntersectRaySphere(Point p, Vector direction, Sphere* sphere);
Color TraceRay(Point O, Vector d, double t_min, double t_max, Sphere scene[]);

#endif // !_MATH_H
