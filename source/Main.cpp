#include <cmath>
#include <string>
#define RAYLIB_IMPLEMENTATION
#include <raylib.h>

#include "Program.hpp"
#include "Drawpad.hpp"
#include "Quadtree.hpp"
#include "DebugRenderer.hpp"
#include "ImageGridEnvironment.hpp"
#include "AstarGraph.hpp"


bool isGameEnd;
bool quadtreeBuild;
bool quadtreeRender;
int maxLevel;

Drawpad drawpad;
Quadtree quadtree;
AstarGraph astarGraph;
DebugRenderer debugRenderer;
ImageGridEnvironment grid;


// Main loop initialization
void Init() {
    //SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    SetTraceLogLevel(LOG_ERROR); 
    SetConfigFlags(FLAG_WINDOW_TRANSPARENT); 
    SetTargetFPS(60);
    InitWindow(WINDOW_W, WINDOW_H, WINDOW_N);
    

    quadtree.Init(WINDOW_W);
    debugRenderer.Init();
    drawpad.Init();
    grid.Init(drawpad.GetPixels(), WINDOW_H, WINDOW_H);

    isGameEnd = false;
    quadtreeBuild = false;
    quadtreeRender = false;
    
    maxLevel = std::log2(WINDOW_W);
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

    if (IsKeyPressed(KEY_Z)) {
        maxLevel = maxLevel - 1 > 0 ? maxLevel - 1 : maxLevel;
        quadtreeBuild = true;
    }    

    if (IsKeyPressed(KEY_X)) {
        maxLevel = maxLevel + 1 > std::log2(WINDOW_W) ? std::log2(WINDOW_W) : maxLevel + 1;
        quadtreeBuild = true;
    }    

    drawpad.Input();
}

// Main loop update
void Update(float deltaTime) {
    if (quadtreeBuild) {
        quadtree.Build(grid, maxLevel);
        astarGraph.Build(quadtree);
        debugRenderer.Update(quadtree, astarGraph);
    }
    
    drawpad.Update();

}


// Main loop draw
void Render() {
    BeginDrawing();
    ClearBackground(BLANK);

    drawpad.Render();

    if (quadtreeRender) {
        debugRenderer.Render();
    }
    
    DrawText(("Max Level: " + std::to_string(maxLevel)).c_str(), 0, 0, 24, DARKBLUE);

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