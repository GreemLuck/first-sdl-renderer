#include "Source.h"

int main(int argc, char* argv[]) {
    SDL_Event event;
    SDL_Renderer* renderer;
    SDL_Window* window;
    bool running;

    // Create window and renderer
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_WIDTH, 0, &window, &renderer);

    // Camera setup

    Point O = { 0,0,0 };

    // Scene setup

    Sphere s1 = { { 0, 1, 3 }, 1, { 255, 0, 0 } };
    Sphere s2 = { { 2, 0, 4 }, 1, { 0, 0, 255 } };
    Sphere s3 = { { -2, 0, 4 }, 1, { 0, 255, 0 } };
    Sphere s4 = { { 0, -5000, 0 }, 5000, { 0, 255, 255 } };

    Sphere scene[] = { s1,s2,s3,s4 };

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
                Color color = TraceRay(O, NewVector(O, d), 1, FLT_MAX, scene);
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
    vpcoord.x = x * ((double)VW / WINDOW_WIDTH);
    vpcoord.y = y * ((double)VH / WINDOW_WIDTH);
    vpcoord.z = (double)D;
    return vpcoord;
}

void PutPixel(int x, int y, Color color, SDL_Renderer* renderer)
{
    x += WINDOW_WIDTH / 2;
    y = (-y) + WINDOW_WIDTH / 2;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_RenderDrawPoint(renderer, x, y);
}