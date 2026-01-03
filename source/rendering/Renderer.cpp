#include <raylib.h>
#include <vector>

#include "Renderer.hpp"


void Renderer::DrawQuadrant(const Quadrant& quad) {
    DrawRectangleLines(quad.GetX() + 1, quad.GetY(), quad.GetWidth() - 1, quad.GetHeight() - 1, PURPLE);
}


void Renderer::DrawQuadtreeLeafs(const Quadtree& quadtree) {
    const std::vector<Quadrant>& leafs = quadtree.GetLeafs();

    for (int i = 0 ; i < leafs.size(); ++i) {
        Renderer::DrawQuadrant(leafs[i]);
    }
}
