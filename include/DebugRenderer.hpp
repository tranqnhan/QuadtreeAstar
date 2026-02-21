#pragma once

#include <raylib.h>

#include "Quadtree.hpp"

class DebugRenderer {
public:
    void Init();
    void DrawQuadrant(const Quadrant& quad, int resolution);
    void UpdateQuadtreeLeafs(const Quadtree& quadtree);
    void DrawQuadtreeLeafs();

    ~DebugRenderer();

private:
    RenderTexture quadtreeLeafsTexture;
};