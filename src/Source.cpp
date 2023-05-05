#include "Source.h"
#include <vector>
#include <thread>
#include <cmath>

void renderChunk(int startX, int endX, int startY, int endY, std::vector<Color>& pixel_colors, Camera camera) {
    for (int x = startX; x < endX; x++) {
        for (int y = startY; y < endY; y++) {
            Vector d = camera.get_rotation_matrix() * c2vp(x, y);
            Color color = TraceRay(camera.position, d, 1, FLT_MAX, RECURSION_LIMIT);
            int xi = x + WINDOW_WIDTH / 2;
            int yi = y + WINDOW_HEIGHT / 2;
            size_t index = (WINDOW_HEIGHT - 1 - yi) * WINDOW_WIDTH + xi;
            pixel_colors[index] = color;
        }
    }
}

int main(int argc, char* argv[]) {
    SDL_Event event;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Window* window;
    bool running;

    // thread setup
    const int NUM_THREADS = std::thread::hardware_concurrency();
    std::vector<std::thread> threads(NUM_THREADS);
    int chunkSize = (WINDOW_HEIGHT / NUM_THREADS);

    // camera setup
    Camera camera;
    camera.position = { 0, 1, -5 };
    camera.pitch = 0;
    camera.yaw = 0;
    camera.roll = 0;
    double camera_distance = 5;
    double camera_rotation_speed = 0.5;

    std::vector<Color> pixel_colors(WINDOW_WIDTH * WINDOW_HEIGHT);

    // Create window and renderer
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer);

    // Create the texture that will serve as the canvas
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);

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
        // animation
        double period = 5.0;
        double amplitude = 1.0;

        int k = (SDL_GetTicks() - now);
        double t = static_cast<double>(k) / 1000.0; // converts to seconds

        // move the red sphere
        double m = amplitude * sin((2 * M_PI / period) * t);
        scene[0].center.y = m;

        // move the camera
        double angle = fmod(camera_rotation_speed * t, 2 * M_PI);
        Point newPos = {
            O.x + camera_distance * cos(angle),
            1,
            O.z + camera_distance * sin(angle)
        };
        camera.position = newPos;
        camera.yaw = -angle - (M_PI/2);
        
        // clear
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        // update pixel data
        for (int i = 0; i < NUM_THREADS; i++) {
            int startY = -WINDOW_HEIGHT / 2 + i * chunkSize;
            int endY = startY + chunkSize;

            if (i == NUM_THREADS - 1) {
                endY = WINDOW_HEIGHT / 2;
            }

            threads[i] = std::thread(renderChunk, -WINDOW_WIDTH / 2, WINDOW_WIDTH / 2, startY, endY, std::ref(pixel_colors), camera);
        }

        for (std::thread& t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }


        // update texture
        SDL_UpdateTexture(texture, NULL, pixel_colors.data(), WINDOW_WIDTH * 4);
        
        //display
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}

Vector c2vp(int x, int y) 
{
    Vector vpcoord;
    vpcoord.x = x * ((double)VW / WINDOW_WIDTH);
    vpcoord.y = y * ((double)VH / WINDOW_HEIGHT);
    vpcoord.z = (double)D; // D is the distance between camera and vp
    return vpcoord;
}

void PutPixel(int x, int y, Color color, SDL_Renderer* renderer)
{
    x += WINDOW_WIDTH / 2;
    y = (-y) + WINDOW_HEIGHT / 2;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_RenderDrawPoint(renderer, x, y);
}

Matrix3 Camera::get_rotation_matrix()
{
    // Pitch (X-axis) rotation matrix
    std::array<std::array<double, 3>, 3> Rx = {{
        { 1,          0,           0 },
        { 0, cos(pitch), -sin(pitch) },
        { 0, sin(pitch),  cos(pitch) }
    }};
    Matrix3 mRx(Rx);

    std::array<std::array<double, 3>, 3> Ry = { {
        {  cos(yaw), 0, sin(yaw) },
        {         0, 1,        0 },
        { -sin(yaw), 0, cos(yaw) }
    } };
    Matrix3 mRy(Ry);

    std::array<std::array<double, 3>, 3> Rz = { {
        { cos(roll), -sin(roll), 0 },
        { sin(roll),  cos(roll), 0 },
        {        0,          0, 1 }
    } };
    Matrix3 mRz(Rz);

    return mRz * mRy * mRx;
}
