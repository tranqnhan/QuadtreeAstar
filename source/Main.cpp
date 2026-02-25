#include <cmath>
#include <cstdint>
#include <string>
#define RAYLIB_IMPLEMENTATION
#include <raylib.h>

#include "Program.hpp"
#include "Drawpad.hpp"
#include "Quadtree.hpp"
#include "DebugRenderer.hpp"
#include "ImageGridEnvironment.hpp"
#include "AstarGraph.hpp"
#include "AstarSearch.hpp"


bool isGameEnd;
bool quadtreeBuild;
bool quadtreeRender;
int maxLevel;

int fromPosX, fromPosY;
int toPosX, toPosY;

bool pathRender;

Drawpad drawpad;
Quadtree quadtree;
AstarGraph astarGraph;
AstarSearch astarSearch;
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
    quadtreeBuild = true;
    quadtreeRender = true;
    
    maxLevel = std::log2(WINDOW_W);
    
    fromPosX = 0;
    fromPosY = 0;

    toPosX = 0;
    toPosY = 0;
}


// Main loop input
void Input() {
    
    
    if (IsKeyPressed(KEY_SPACE)) {
        isGameEnd = true;
        return;
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        quadtreeBuild = true;
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        quadtreeRender = false;
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        pathRender = true;
        quadtreeRender = true;
    }

    if (IsKeyPressed(KEY_Z)) {
        maxLevel = maxLevel - 1 > 0 ? maxLevel - 1 : maxLevel;
        quadtreeBuild = true;
        pathRender = true;
    }    

    if (IsKeyPressed(KEY_X)) {
        maxLevel = maxLevel + 1 > std::log2(WINDOW_W) ? std::log2(WINDOW_W) : maxLevel + 1;
        quadtreeBuild = true;
        pathRender = true;
    }    
    
    if (IsKeyPressed(KEY_Q)) {
        Vector2 mousePosition = GetMousePosition();
        fromPosX = mousePosition.x;
        fromPosY = mousePosition.y;
        pathRender = true;
    }

    if (IsKeyPressed(KEY_W)) {
        Vector2 mousePosition = GetMousePosition();
        toPosX = mousePosition.x;
        toPosY = mousePosition.y;
        pathRender = true;
    }

    drawpad.Input();
}

// Main loop update
void Update(float deltaTime) {
    if (quadtreeBuild) {
        quadtree.Build(grid, maxLevel);
        astarGraph.Build(quadtree);
        debugRenderer.Update(quadtree, astarGraph);
        quadtreeBuild = false;
    }

    if (pathRender) {
        const std::vector<int>& path = astarSearch.GetPath(quadtree, astarGraph, fromPosX, fromPosY, toPosX, toPosY);
        debugRenderer.UpdatePath(path);
        pathRender = false;
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