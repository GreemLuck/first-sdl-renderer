#include "RenderMath.h"

Color BACKGROUND_COLOR = { 0,0,0 };

Vector NewVector(Point p)
{
	return { p.x, p.y, p.z };
}

Vector NewVector(Point p1, Point p2)
{
	return { p2.x - p1.x, p2.y - p1.y, p2.z - p1.z };
}

double GetNorm(Vector v)
{
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vector Normalize(Vector v) 
{
	double norm = GetNorm(v);
	return {v.x / norm, v.y / norm, v.z / norm};
}

double Dot(Vector v1, Vector v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

std::tuple<double, double> IntersectRaySphere(Point p, Vector direction, Sphere* sphere)
{
    double r = sphere->radius;
    Vector CP = NewVector(sphere->center, p);

    double a = Dot(direction, direction);
    double b = 2 * Dot(CP, direction);
    double c = Dot(CP, CP) - r * r;

    double discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
        return { HUGE_VAL, HUGE_VAL };
    }

    double t1 = (-b + sqrt(discriminant)) / (2 * a);
    double t2 = (-b - sqrt(discriminant)) / (2 * a);
    return { t1, t2 };
}

Color TraceRay(Point O, Vector d, double t_min, double t_max, Sphere scene[])
{
    double closest_t = HUGE_VAL;
    Sphere* closest_sphere = nullptr;
    for (int i = 0; i < 4; i++)
    {
        auto [t1, t2] = IntersectRaySphere(O, d, &scene[i]);
        if (t_min < t1 && t1 < t_max && t1 < closest_t)
        {
            closest_t = t1;
            closest_sphere = &scene[i];
        }
        if (t_min < t2 && t2 < t_max && t2 < closest_t)
        {
            closest_t = t2;
            closest_sphere = &scene[i];
        }
    }
    if (closest_sphere == nullptr) {
        return BACKGROUND_COLOR;
    }
    return closest_sphere->color;
}