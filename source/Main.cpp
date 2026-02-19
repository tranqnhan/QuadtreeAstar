#define RAYLIB_IMPLEMENTATION
#include <raylib.h>

#include "Program.hpp"
#include "Drawpad.hpp"
#include "Quadtree.hpp"
#include "QuadtreeRenderer.hpp"
#include "ImageGridEnvironment.hpp"


bool isGameEnd;
bool quadtreeBuild;
bool quadtreeRender;

Drawpad drawpad;
Quadtree quadtree;
QuadtreeRenderer quadtreeRenderer;
ImageGridEnvironment grid;


// Main loop initialization
void Init() {
    //SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    SetTraceLogLevel(LOG_ERROR); 
    SetConfigFlags(FLAG_WINDOW_TRANSPARENT); 
    SetTargetFPS(60);
    InitWindow(WINDOW_W, WINDOW_H, WINDOW_N);
    

    quadtree.Init(WINDOW_W);
    quadtreeRenderer.Init();
    drawpad.Init();
    grid.Init(drawpad.GetPixels(), WINDOW_H, WINDOW_H);

    quadtreeRender = false;
    quadtreeBuild = false;
    isGameEnd = false;
}


// Main loop input
void Input() {
    if (IsKeyPressed(KEY_SPACE)) {
        isGameEnd = true;
        return;
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        quadtreeBuild = true;
    } else {
        quadtreeBuild = false;
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        quadtreeRender = false;
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        quadtreeRender = true;
    }

    drawpad.Input();
}

// Main loop update
void Update(float deltaTime) {
    if (quadtreeBuild) {
        quadtree.Build(grid);
        quadtreeRenderer.UpdateQuadtreeLeafs(quadtree);
    }
    
    drawpad.Update();

}


// Main loop draw
void Render() {
    BeginDrawing();
    ClearBackground(BLANK);

    drawpad.Render();

    if (quadtreeRender) {
        quadtreeRenderer.DrawQuadtreeLeafs();
    }

    DrawFPS(0, 0);

    EndDrawing();
}


// Main loop
int main(int argc, char* argv[]) {
    Init();

    while (!WindowShouldClose() && !isGameEnd) {
        Input();
        Update(GetFrameTime());
        Render();
    }

    CloseWindow();
    return 0;
}