#pragma once

#include <raylib.h>

#include "Quadtree.hpp"

class Renderer {
public:
    static void Init();
    static void DrawQuadrant(const Quadrant& quad);
    static void UpdateQuadtreeLeafs(const Quadtree& quadtree);
    static void DrawQuadtreeLeafs();

private:
    static RenderTexture quadtreeLeafsTexture;
};