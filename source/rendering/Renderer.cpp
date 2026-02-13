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
    int length = 1 << (resolution - quad.GetLevel());
    int x = quad.GetX(); //(512 - quad.GetX()) - length;
    int y = quad.GetY(); //(512 - quad.GetY()) - length;

    DrawRectangleLines(x, y, length, length, PURPLE);

    uint64_t code = quad.GetCode();
    std::string codeText = "";

    while (resolution > 0) {
        codeText += std::to_string(code & 3);
        code >>= 2;
        resolution -= 1;
    }
    std::reverse(codeText.begin(), codeText.end());

    //DrawText(codeText.c_str(), x, y, 12, BLUE);
}


void Renderer::UpdateQuadtreeLeafs(const Quadtree& quadtree) {
    BeginTextureMode(Renderer::quadtreeLeafsTexture);
    const std::vector<Quadrant>& leafs = quadtree.GetLeafs();
    const std::vector<std::vector<int>>& graph = quadtree.GetGraph();

    for (int i = 0 ; i < leafs.size(); ++i) {
        
        Renderer::DrawQuadrant(leafs[i], quadtree.GetResolution());
        
        //int x1 = leafs[i].GetX() + leafs[i].GetWidth() / 2;
        //int y1 = leafs[i].GetY() + leafs[i].GetHeight() / 2;
        int length1 = 1 << (quadtree.GetResolution() - leafs[i].GetLevel());
        int x1 = leafs[i].GetX() + (length1 / 2); //(512 - quad.GetX()) - length;
        int y1 = leafs[i].GetY() + (length1 / 2); //(512 - quad.GetY()) - length;
    
        for (int k = 0; k < graph[i].size(); ++k) {
            int adjIndex = graph[i][k];
            int length2 = 1 << (quadtree.GetResolution() - leafs[adjIndex].GetLevel());
            int x2 = leafs[adjIndex].GetX() + length2 / 2;
            int y2 = leafs[adjIndex].GetY() + length2 / 2;
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
