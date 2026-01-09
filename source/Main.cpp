#include <raylib.h>

#include "GridEnvironment.hpp"
#include "Quadtree.hpp"
#include "Renderer.hpp"


#define WINDOW_W 512
#define WINDOW_H 512
#define WINDOW_N "QUADTREE ASTAR"


Quadtree quadtree(3);

Image image;
Texture2D texture;

// Main loop initialization
void Init() {
    //SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    SetTargetFPS(60);
    InitWindow(WINDOW_W, WINDOW_H, WINDOW_N);
    
    image = LoadImage("../assets/test.png");

    GridEnvironment grid(image.width, image.height);

    Color *pixels = LoadImageColors(image);
    for (int i = 0; i < image.width * image.height; ++i) {
        grid.SetValid(i, pixels[i].b == 255);
    }

    texture = LoadTextureFromImage(image);


    quadtree.Build(grid);
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
    DrawTexture(texture, 0, 0, WHITE);

    Renderer::DrawQuadtreeLeafs(quadtree);


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