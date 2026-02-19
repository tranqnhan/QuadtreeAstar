#pragma once

#include <raylib.h>

#include "Quadtree.hpp"

class QuadtreeRenderer {
public:
    void Init();
    void DrawQuadrant(const Quadrant& quad, int resolution);
    void UpdateQuadtreeLeafs(const Quadtree& quadtree);
    void DrawQuadtreeLeafs();

    ~QuadtreeRenderer();

private:
    RenderTexture quadtreeLeafsTexture;
};