#pragma once
#ifndef _SOURCE_H
#define _SOURCE_H

#include <stdlib.h>
#include <cfloat>
#include <SDL.h>
#include "RenderMath.h"

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600
#define VW 1
#define VH 1
#define D 1

struct Camera {
    Point position;
    double pitch;
    double yaw;
    double roll;
    float fieldOfView;
    float aspectRatio;
    float nearClip;
    float farClip;
    Matrix3 get_rotation_matrix();
};

Vector c2vp(int x, int y);
void PutPixel(int x, int y, Color color, SDL_Renderer* renderer);

#endif