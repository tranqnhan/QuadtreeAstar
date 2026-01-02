#include <raylib.h>
#include <vector>

#include "Quadtree.hpp"
#include "Renderer.hpp"


#define WINDOW_W 700
#define WINDOW_H 700
#define WINDOW_N "QUADTREE ASTAR"


std::vector<Quadrant> groupQuads;


// Main loop initialization
void Init() {
    SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    SetTargetFPS(60);
    InitWindow(WINDOW_W, WINDOW_H, WINDOW_N);

}


// Main loop update
void Update(float deltaTime) {

}


// Main loop input
void Input() {

}


// Main loop draw
void Draw() {
    BeginDrawing();
    ClearBackground(BLACK);

    DrawFPS(0, 0);

    // Draw
    for (Quadrant quad : groupQuads) {
        Renderer::DrawQuadrant(quad);
    }

    EndDrawing();
}


// Main loop
int main() {
    Init();

    while (!WindowShouldClose()) {
        Input();
        Update(GetFrameTime());
        Draw();
    }

    CloseWindow();
    return 0;
}