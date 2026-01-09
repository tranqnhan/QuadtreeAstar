#include <vector>

#include <raylib.h>

#include "Program.hpp"
#include "Renderer.hpp"

RenderTexture Renderer::quadtreeLeafsTexture;

void Renderer::Init() {
    Renderer::quadtreeLeafsTexture = LoadRenderTexture(WINDOW_W, WINDOW_H);
}


void Renderer::DrawQuadrant(const Quadrant& quad) {
    DrawRectangleLines(quad.GetX() + 1, quad.GetY(), quad.GetWidth() - 1, quad.GetHeight() - 1, PURPLE);
}


void Renderer::UpdateQuadtreeLeafs(const Quadtree& quadtree) {
    BeginTextureMode(Renderer::quadtreeLeafsTexture);
    const std::vector<Quadrant>& leafs = quadtree.GetLeafs();

    for (int i = 0 ; i < leafs.size(); ++i) {
        Renderer::DrawQuadrant(leafs[i]);
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
