#include <chrono>
#include <iostream>

#include <raylib.h>

#include "Program.hpp"
#include "GridEnvironment.hpp"
#include "Quadtree.hpp"
#include "Renderer.hpp"

Quadtree quadtree(32);

Image image;
Texture2D texture;

// Main loop initialization
void Init() {
    //SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    SetTraceLogLevel(LOG_ERROR); 
    SetTargetFPS(30);
    InitWindow(WINDOW_W, WINDOW_H, WINDOW_N);
    
    Renderer::Init();

    image = LoadImage("../assets/test4.png");

    GridEnvironment grid(image.width, image.height);

    Color *pixels = LoadImageColors(image);
    for (int i = 0; i < image.width * image.height; ++i) {
        grid.SetValid(i, pixels[i].b > 0);
    }

    texture = LoadTextureFromImage(image);


    auto start = std::chrono::high_resolution_clock::now();
    quadtree.Build(grid);
    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    std::cout << duration.count() << std::endl;

    Renderer::UpdateQuadtreeLeafs(quadtree);

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

    // Draw
    DrawTexture(texture, 0, 0, WHITE);

    Renderer::DrawQuadtreeLeafs();

    DrawRectangle(0, 0, 100, 25, BLACK);
    DrawFPS(0, 0);
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