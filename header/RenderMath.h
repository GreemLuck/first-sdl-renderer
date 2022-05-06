#pragma once
#ifndef _RENDERMATH_H
#define _RENDERMATH_H

#include <stdlib.h>
#include <math.h>
#include <tuple>

extern int RECURSION_LIMIT;

typedef struct Vector {
	double x, y, z;
	Vector operator*(double);
	Vector operator-(Vector);
} Vector;

typedef struct Point {
	double x, y, z;
	Point operator+(Point);
	Point operator+(Vector);
} Point;

typedef struct Color {
	int r, g, b, a;
	Color operator*(double);
	Color operator+(Color);
} Color;

Vector NewVector(Point);
Vector NewVector(Point, Point);
double GetNorm(Vector); 
Vector Normalize(Vector);
double Dot(Vector, Vector);
Vector ReflectRay(Vector, Vector);

typedef struct Sphere {
	Point center;
	double radius;
	Color color;
	double specular;
	double reflectiveness;
} Sphere;

std::tuple<double, double> IntersectRaySphere(Point, Vector, Sphere*);
std::tuple<Sphere*, double> ClosestIntersection(Point, Vector, double, double);
Color TraceRay(Point , Vector , double , double, int);

double ComputeAmbientLight();
double ComputePointLight(Point, Vector, Vector, double);
double ComputeDirectionalLight(Point, Vector, Vector, double);
double ComputeLighting(Point, Vector, Vector, double);
double ComputeSpecular(Vector, Vector, Vector, double, double);

extern Point O;
extern Sphere scene[];


#endif // !_MATH_H
