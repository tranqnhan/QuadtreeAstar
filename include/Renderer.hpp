#pragma once

#include <raylib.h>

#include "Quadtree.hpp"

class Renderer {
public:
    static void Init();
    static void DrawQuadrant(const Quadrant& quad, int resolution);
    static void UpdateQuadtreeLeafs(const Quadtree& quadtree);
    static void DrawQuadtreeLeafs();

private:
    static Image quadtreeImage;
    static RenderTexture quadtreeLeafsTexture;
};