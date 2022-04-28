#include <stdlib.h>
#include <cfloat>
#include <SDL.h>

#define WINDOW_WIDTH 600
#define VW 1
#define VH 1
#define D 1

typedef struct Point {
    float x, y, z;
} Point;

typedef struct Color {
    Uint8 r, g, b;
} Color;

typedef struct Sphere {
    Point center;
    float radius;
    Color color;
} Sphere;

typedef struct Vector3 {
    float x, y, z;
} Vector3;

Color BACKGROUND_COLOR = { 0,0,0 };

Point c2vp(int x, int y);
void PutPixel(int x, int y, Color color, SDL_Renderer* renderer);
Color TraceRay(Point O, Vector3 d, float t_min, float t_max, Sphere scene[]);
auto IntersectRaySphere(Point O, Vector3 d, Sphere* sphere);
float dot(Vector3 a, Vector3 b);
Vector3 point2vector(Point a, Point b);

int main(int argc, char* argv[]) {
    SDL_Event event;
    SDL_Renderer* renderer;
    SDL_Window* window;
    bool running;

    // Create window and renderer
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_WIDTH, 0, &window, &renderer);

    // Camera setup

    Point O;
    O.x = 0;
    O.y = 0;
    O.z = 0;

    // Scene setup

    Point center1 = { 0, 1, 3 };
    float r1 = 1;
    Color color1 = { 255, 0 ,0 };
    
    Point center2 = { 2, 0, 4 };
    float r2 = 1;
    Color color2 = { 0, 0, 255 };

    Point center3 = { -2, 0, 4 };
    float r3 = 1;
    Color color3 = { 0, 255, 0 };

    Sphere s1 = { center1, r1, color1 };
    Sphere s2 = { center2, r2, color2 };
    Sphere s3 = { center3, r3, color3 };

    Sphere scene[] = { s1,s2,s3 };

    // Render loop
    running = true;
    while (running) {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                {
                    running = false;
                } break;

                default: {} break;
            }
        }

        // clear
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        // update
        //render
        for (int x = -WINDOW_WIDTH / 2; x < WINDOW_WIDTH / 2; x++)
        {
            for (int y = -WINDOW_WIDTH / 2; y < WINDOW_WIDTH / 2; y++)
            {
                Point d = c2vp(x, y);
                Color color = TraceRay(O, point2vector(O, d), 1, FLT_MAX, scene);
                // Color color = { 0, 120, 0 };
                PutPixel(x, y, color, renderer);
            }
        }
        
        //display
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}

Point c2vp(int x, int y) 
{
    Point vpcoord;
    vpcoord.x = x * ((float)VW / WINDOW_WIDTH);
    vpcoord.y = y * ((float)VH / WINDOW_WIDTH);
    vpcoord.z = (float)D;
    return vpcoord;
}

void PutPixel(int x, int y, Color color, SDL_Renderer* renderer)
{
    x += WINDOW_WIDTH / 2;
    y = (-y) + WINDOW_WIDTH / 2;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_RenderDrawPoint(renderer, x, y);
}

auto IntersectRaySphere(Point O, Vector3 d, Sphere* sphere)
{
    struct result { float t1, t2; };
    float r = sphere->radius;
    Vector3 CO = point2vector(O, sphere->center);

    float a = dot(d, d);
    float b = 2 * dot(CO, d);
    float c = dot(CO, CO) - r * r;

    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
        return result{ FLT_MAX, FLT_MAX };
    }

    float t1 = -(float)(-b + sqrt((double)discriminant)) / (2 * a);
    float t2 = -(float)(-b - sqrt((double)discriminant)) / (2 * a);
    return result{ t1, t2 };
}

Color TraceRay(Point O, Vector3 d, float t_min, float t_max, Sphere scene[])
{
    float closest_t = FLT_MAX;
    Sphere* closest_sphere = nullptr;
    for (int i = 0; i < 3; i++)
    {
        auto result = IntersectRaySphere(O, d, &scene[i]);
        float t1 = result.t1;
        float t2 = result.t2;
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



Point substractPoint(Point a, Point b)
{
    Point res;
    res.x = a.x - b.x;
    res.y = a.y - b.y;
    res.z = a.z - b.z;
    return res;
}

float dot(Vector3 a, Vector3 b) 
{
    float res = 0;
    res += a.x * b.x + a.y * b.y + a.z * b.z;
    return res;
}

Vector3 point2vector(Point a, Point b) 
{
    Vector3 res;
    res.x = b.x - a.x;
    res.y = b.y - a.y;
    res.z = b.z - a.z;
    return res;
}