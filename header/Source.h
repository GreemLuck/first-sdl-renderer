#pragma once
#ifndef _SOURCE_H
#define _SOURCE_H

#include <stdlib.h>
#include <cfloat>
#include <SDL.h>
#include "RenderMath.h"

#define WINDOW_WIDTH 600
#define VW 1
#define VH 1
#define D 1

Point c2vp(int x, int y);
void PutPixel(int x, int y, Color color, SDL_Renderer* renderer);

#endif