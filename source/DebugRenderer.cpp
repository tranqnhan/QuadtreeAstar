#include <vector>

#include <raylib.h>

#include "Program.hpp"
#include "Quadtree.hpp"
#include "DebugRenderer.hpp"


void DebugRenderer::Init() {
    quadtreeLeafsTexture = LoadRenderTexture(WINDOW_W, WINDOW_H);
}


void DebugRenderer::DrawQuadrant(const Quadrant& quad, int resolution) {
    const int length = 1 << (resolution - quad.GetLevel());
    const int x = quad.GetX(); //(512 - quad.GetX()) - length;
    const int y = quad.GetY(); //(512 - quad.GetY()) - length;

    const Color color = quad.IsValid() ? PURPLE : RED;

    DrawRectangleLines(x, y, length, length, color);

    // std::string codeText = "";
    // uint64_t code = quad.GetCode();

    // while (resolution > 0) {
    //     codeText += std::to_string(code & 3);
    //     code >>= 2;
    //     resolution -= 1;
    // }
    // std::reverse(codeText.begin(), codeText.end());

    // DrawText(codeText.c_str(), x, y, 12, BLUE);
}


void DebugRenderer::UpdateQuadtreeLeafs(const Quadtree& quadtree) {
    // Renderer::quadtreeImage = GenImageColor(WINDOW_W, WINDOW_H, WHITE);

    BeginTextureMode(quadtreeLeafsTexture);
    ClearBackground(BLANK);

    const std::vector<Quadrant>& leafs = quadtree.GetLeafs();
    const std::vector<std::vector<int>>& graph = quadtree.GetGraph();

    if (quadtree.GetGraph().size() > 0) {
        for (int i = 0 ; i < leafs.size(); ++i) {
            
            DrawQuadrant(leafs[i], quadtree.GetResolution());
            
            const int length1 = 1 << (quadtree.GetResolution() - leafs[i].GetLevel());
            const int x1 = leafs[i].GetX() + (length1 / 2); 
            const int y1 = leafs[i].GetY() + (length1 / 2); 
        
            for (int k = 0; k < graph[i].size(); ++k) {
                const int adjIndex = graph[i][k];
                const int length2 = 1 << (quadtree.GetResolution() - leafs[adjIndex].GetLevel());
                const int x2 = leafs[adjIndex].GetX() + length2 / 2;
                const int y2 = leafs[adjIndex].GetY() + length2 / 2;
                DrawLine(x1, y1, x2, y2, BLUE);
            } 
        }
    }

    //UpdateTexture(Renderer::quadtreeLeafsTexture.texture, Renderer::quadtreeImage.data); // Initial upload

    EndTextureMode();
}


void DebugRenderer::DrawQuadtreeLeafs() {
    DrawTextureRec(
    quadtreeLeafsTexture.texture, 
    (Rectangle) {
        .x = 0,
        .y = 0,
        .width = WINDOW_W,
        .height = -WINDOW_H,
    }, (Vector2) {0, 0}, WHITE);
}


DebugRenderer::~DebugRenderer() {
    UnloadRenderTexture(quadtreeLeafsTexture);
}