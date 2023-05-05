#include "RenderMath.h"

Color BACKGROUND_COLOR = { 0,5,20 };
int RECURSION_LIMIT = 3;

// Ambient, Point and Directional Ligh settings
double AL_INTENSITY = 0.0;
double PL_INTENSITY = 0.8;
Point PL_POSITION = { 0,5,0 };
double DL_INTENSITY = 0.9;
Vector DL_DIRECTION = { 1, 4, 4 };


// Camera setup

Point O = { 0,0,0 };

// Scene setup

Sphere s1 = { { 0, -1, 0 }, 1, { 255, 0, 0 }, 500, 0.5 }; // Red
Sphere s2 = { { 3, 0, 0 }, 1, { 0, 0, 255 }, 500, 0.4 }; // Blue
Sphere s3 = { { -3, 0, 0 }, 1, { 0, 255, 0 }, 20, 0.1 }; // Green
Sphere s4 = { { 0, -5001, 0 }, 5000, { 0, 255, 255 }, 1000 , 0.7}; // Yellow

Sphere scene[] = { s1,s2,s3,s4 };

// POINTS

Point Point::operator+(Point other) {
    return { this->x + other.x, this->y + other.y, this->z + other.z };
}

Point Point::operator+(Vector v) {
    return { this->x + v.x, this->y + v.y, this->z + v.z };
}

// COLORS

Color Color::operator*(double c) {
    int r = static_cast<int>(this->r) * c;
    int g = static_cast<int>(this->g) * c;
    int b = static_cast<int>(this->b) * c;
        return { static_cast<uint8_t>((r > 255) ? 255 : r),
                 static_cast<uint8_t>((g > 255) ? 255 : g),
                 static_cast<uint8_t>((b > 255) ? 255 : b),
                 this->a };
}

Color Color::operator+(Color other) {
    int r = static_cast<int>(this->r) + static_cast<int>(other.r);
    int g = static_cast<int>(this->g) + static_cast<int>(other.g);
    int b = static_cast<int>(this->b) + static_cast<int>(other.b);

    return { static_cast<uint8_t>((r > 255) ? 255 : r),
             static_cast<uint8_t>((g > 255) ? 255 : g),
             static_cast<uint8_t>((b > 255) ? 255 : b),
             this->a };
}

// VECTORS

Vector Vector::operator*(double c) {
    return { this->x * c, this->y * c, this->z * c };
}

Vector Vector::operator-(Vector other) {
    return { this->x - other.x, this->y - other.y, this->z - other.z };
}

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

Vector ReflectRay(Vector r, Vector n) {
    return n * 2 * Dot(n, r) - r;
}

// MATH

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

std::tuple<Sphere*, double> ClosestIntersection(Point o, Vector d, double t_min, double t_max) {
    double closest_t = HUGE_VAL;
    Sphere* closest_sphere = nullptr;
    for (int i = 0; i < 4; i++)
    {
        auto [t1, t2] = IntersectRaySphere(o, d, &scene[i]);
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
    return { closest_sphere, closest_t };
}

Color TraceRay(Point O, Vector d, double t_min, double t_max, int recursion_depth)
{
    auto [closest_sphere, closest_t] = ClosestIntersection(O, d, t_min, t_max);
    if (closest_sphere == nullptr) {
        return BACKGROUND_COLOR;
    }

    // Compute local color
    Point p = O + d * closest_t; // Compute intersection
    Vector n = NewVector(closest_sphere->center, p); // Compute sphere normal at intersection
    n = Normalize(n);
    Color local_color = closest_sphere->color * ComputeLighting(p, n, d*(-1), closest_sphere->specular);

    // If we hit recursion limit or the object is not reflective, we are done
    double r = closest_sphere->reflectiveness;
    if (recursion_depth <= 0 || r <= 0) {
        return local_color;
    }

    // Compute the reflected color
    Vector reflected_d = ReflectRay({ -d.x, -d.y, -d.z }, n);
    Color reflected_color = TraceRay(p, reflected_d, 0.0001, t_max, recursion_depth - 1);
    
    return local_color * (1 - r) + reflected_color * r;
}

// LIGHT

double ComputeAmbientLight() {
    return AL_INTENSITY;
}

double ComputePointLight(Point p, Vector n, Vector v, double s) {
    double i = 0;
    Vector l = NewVector(p, PL_POSITION);
    double n_dot_l = Dot(n, l);

    auto [shadow_sphere, shadow_t] = ClosestIntersection(p, l, 0.001, HUGE_VAL);
    if (shadow_sphere != nullptr) {
        return 0;
    }

    // Diffuse
    if (n_dot_l > 0) {
        i += PL_INTENSITY * n_dot_l / (GetNorm(n) * GetNorm(l));
    }

    // Specular
    i += ComputeSpecular(n, l, v, s, PL_INTENSITY);
    return i;
}

double ComputeDirectionalLight(Point p, Vector n, Vector v, double s) {
    double i = 0;
    double n_dot_l = Dot(n, DL_DIRECTION);

    auto [shadow_sphere, shadow_t] = ClosestIntersection(p, DL_DIRECTION, 0.001, HUGE_VAL);
    if (shadow_sphere != nullptr) {
        return 0;
    }

    // Difuse
    if (n_dot_l > 0) {
        i += DL_INTENSITY * n_dot_l / (GetNorm(n) * GetNorm(DL_DIRECTION));
    }

    // Specular
    i += ComputeSpecular(n, DL_DIRECTION, v, s, PL_INTENSITY);
    return i;
}

double ComputeSpecular(Vector n, Vector l, Vector v, double s, double intensity) {
    if (s != -1) {
        Vector r = ReflectRay(n, l);
        double r_dot_v = Dot(r, v);
        if (r_dot_v > 0) {
            return intensity * pow(r_dot_v / (GetNorm(r) * GetNorm(v)), s);
        }
    }
    return 0;
}

double ComputeLighting(Point p, Vector n, Vector v, double s) {
    double i = 0;
    i += ComputeAmbientLight();
    i += ComputeDirectionalLight(p, n, v, s);
    // i += ComputePointLight(p, n, v ,s);
    return i;
}

Matrix3::Matrix3(double value)
{
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            data[i][j] = value;
        }
    }
}

Matrix3::Matrix3(std::array<std::array<double, 3>, 3> data):
    data(data)
{
}

double& Matrix3::operator()(size_t row, size_t col)
{
    return data[row][col];
}

const double& Matrix3::operator()(size_t row, size_t col) const
{
    return data[row][col];
}

Vector Matrix3::operator*(const Vector vec) const
{
    Vector result;
    result.x = data[0][0] * vec.x + data[0][1] * vec.y + data[0][2] * vec.z;
    result.y = data[1][0] * vec.x + data[1][1] * vec.y + data[1][2] * vec.z;
    result.z = data[2][0] * vec.x + data[2][1] * vec.y + data[2][2] * vec.z;
    return result;
}

Matrix3 Matrix3::operator*(const Matrix3 mat) const
{
    Matrix3 result(0);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result.data[i][j] = 0.0;
            for (int k = 0; k < 3; k++) {
                result.data[i][j] += data[i][k] * mat.data[k][j];
            }
        }
    }
    return result;
}
