#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>
#include <bits/stdc++.h>

#include <raylib.h>

#include "Program.hpp"
#include "Quadtree.hpp"
#include "Renderer.hpp"

RenderTexture Renderer::quadtreeLeafsTexture;

void Renderer::Init() {
    Renderer::quadtreeLeafsTexture = LoadRenderTexture(WINDOW_W, WINDOW_H);
}


void Renderer::DrawQuadrant(const Quadrant& quad, int resolution) {
    DrawRectangleLines(quad.GetX() + 1, quad.GetY(), quad.GetWidth() - 1, quad.GetHeight() - 1, PURPLE);

    uint64_t code = quad.GetCode();
    std::string codeText = "";
    std::printf("%lb\n", quad.GetCode());

    while (resolution > 0) {
        codeText += std::to_string(code & 3);
        code >>= 2;
        resolution -= 1;
    }
    std::reverse(codeText.begin(), codeText.end());

    DrawText(codeText.c_str(), quad.GetX() + 1, quad.GetY(), 12, BLUE);
}


void Renderer::UpdateQuadtreeLeafs(const Quadtree& quadtree) {
    BeginTextureMode(Renderer::quadtreeLeafsTexture);
    const std::vector<Quadrant>& leafs = quadtree.GetLeafs();
    const std::vector<std::vector<int>>& graph = quadtree.GetGraph();

    for (int i = 0 ; i < leafs.size(); ++i) {
        Renderer::DrawQuadrant(leafs[i], quadtree.GetResolution());
        
        int x1 = leafs[i].GetX() + leafs[i].GetWidth() / 2;
        int y1 = leafs[i].GetY() + leafs[i].GetHeight() / 2;
            
        for (int k = 0; k < graph[i].size(); ++k) {
            int adjIndex = graph[i][k];
            std::printf("neighbor of %i is %i\n", i, adjIndex);
            int x2 = leafs[adjIndex].GetX() + leafs[adjIndex].GetWidth() / 2;
            int y2 = leafs[adjIndex].GetY() + leafs[adjIndex].GetHeight() / 2;
            DrawLine(x1, y1, x2, y2, BLUE);
        } 
    }
    EndTextureMode();
}


void Renderer::DrawQuadtreeLeafs() {
    DrawTextureRec(
        Renderer::quadtreeLeafsTexture.texture, 
    (Rectangle) {
        .x = 0,
        .y = 0,
        .width = WINDOW_W,
        .height = -(WINDOW_H),
    }, (Vector2) {0, 0}, WHITE);
}
