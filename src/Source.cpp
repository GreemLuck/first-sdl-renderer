#include "Source.h"

int main(int argc, char* argv[]) {
    SDL_Event event;
    SDL_Renderer* renderer;
    SDL_Window* window;
    bool running;

    // Create window and renderer
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_WIDTH, 0, &window, &renderer);

    int delay = 5000; // 5 seconds of delay
    uint32_t now = SDL_GetTicks();

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
        int k = (SDL_GetTicks() - now) % delay;
        double m = (double)k / delay;
        scene[0].center.y = -1 + 2 * m; // trying to move the red sphere
        
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
                Color color = TraceRay(O, NewVector(O, d), 1, FLT_MAX, RECURSION_LIMIT);
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