#pragma once

#include <raylib.h>

#include "AstarGraph.hpp"
#include "Quadtree.hpp"

class DebugRenderer {
public:
    void Init();
    
    void Update(const Quadtree& quadtree, const AstarGraph& astarGraph);
    void UpdateRegionSelect(const Quadtree& quadtree, int regionSelect);
    void Render();

    ~DebugRenderer();

private:
    RenderTexture debugTexture;
    RenderTexture debugRegionTexture;
    
    void DrawQuadrant(const Quadrant& quad, int resolution);
    void UpdateQuadtree(const Quadtree& quadtree);
    void UpdateAstarGraph(const AstarGraph& astarGraph);
};