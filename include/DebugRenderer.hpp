#pragma once

#include <raylib.h>

#include "AstarGraph.hpp"
#include "Quadtree.hpp"

class DebugRenderer {
public:
    void Init();
    
    void Update(const Quadtree& quadtree, const AstarGraph& astarGraph);
    void UpdatePath(const std::vector<int>& path);
    void Render();

    ~DebugRenderer();

private:
    RenderTexture debugTexture;
    RenderTexture debugPathTexture;
    
    void DrawQuadrant(const Quadrant& quad, int resolution);
    void UpdateQuadtree(const Quadtree& quadtree);
    void UpdateAstarGraph(const AstarGraph& astarGraph);
};