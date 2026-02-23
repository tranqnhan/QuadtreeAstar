#include <cstdio>
#include <vector>

#include <raylib.h>

#include "AstarGraph.hpp"
#include "Program.hpp"
#include "Quadtree.hpp"
#include "DebugRenderer.hpp"


void DebugRenderer::Init() {
    debugTexture = LoadRenderTexture(WINDOW_W, WINDOW_H);
    debugRegionTexture = LoadRenderTexture(WINDOW_W, WINDOW_H);

}


void DebugRenderer::DrawQuadrant(const Quadrant& quad, int resolution) {
    const int length = 1 << (resolution - quad.GetLevel());
    const int x = quad.GetX(); //(512 - quad.GetX()) - length;
    const int y = quad.GetY(); //(512 - quad.GetY()) - length;

    const Color color = quad.IsValid() ? PURPLE : RED;

    DrawRectangleLines(x, y, length, length, color);

    // std::string codeText = "";
    // uint64_t code = quad.GetCode();

    // while (resolution > 0) {
    //     codeText += std::to_string(code & 3);
    //     code >>= 2;
    //     resolution -= 1;
    // }
    // std::reverse(codeText.begin(), codeText.end());

    // DrawText(codeText.c_str(), x, y, 12, BLUE);
}

void DebugRenderer::Update(const Quadtree& quadtree, const AstarGraph& astarGraph) {
    BeginTextureMode(debugTexture);
    ClearBackground(BLANK);

    this->UpdateQuadtree(quadtree);
    this->UpdateAstarGraph(astarGraph);


    EndTextureMode();
}

void DebugRenderer::UpdateQuadtree(const Quadtree& quadtree) {
    // Renderer::quadtreeImage = GenImageColor(WINDOW_W, WINDOW_H, WHITE);

    const std::vector<Quadrant>& leafs = quadtree.GetLeafs();
    const std::vector<std::vector<int>>& graph = quadtree.GetGraph();

    for (int i = 0 ; i < leafs.size(); ++i) {
        DrawQuadrant(leafs[i], quadtree.GetResolution());
    }

    //UpdateTexture(Renderer::quadtreeLeafsTexture.texture, Renderer::quadtreeImage.data); // Initial upload

}


void DebugRenderer::UpdateAstarGraph(const AstarGraph& astarGraph) {
    const std::vector<AstarNode>& nodes = astarGraph.GetNodes();
    const std::vector<AstarEdge>& edges = astarGraph.GetEdges();
    
    for (int i = 0; i < nodes.size(); ++i) {
        for (int j = nodes[i].GetEdgeIndex(); j < nodes[i].GetEdgeIndex() + nodes[i].GetNumEdges(); j++) {
            const int startPosX = nodes[i].GetX();
            const int startPosY = nodes[i].GetY();
            const int endPosX = nodes[edges[j].GetNodeIdB()].GetX();
            const int endPosY = nodes[edges[j].GetNodeIdB()].GetY();
        
            DrawLine(startPosX, startPosY, endPosX, endPosY, BLUE);
        }
        //std::printf("index %i\n", nodes[i].GetEdgeIndex());
        //std::fflush(stdout);
    }

}


void DebugRenderer::UpdateRegionSelect(const Quadtree& quadtree, int regionSelect) {

    BeginTextureMode(debugRegionTexture);
    ClearBackground(BLANK);


    if (regionSelect >= 0) {
        const Quadrant& quad = quadtree.GetLeafs()[regionSelect];
        const int length = 1 << (quadtree.GetResolution() - quad.GetLevel());
        const int x = quad.GetX(); //(512 - quad.GetX()) - length;
        const int y = quad.GetY(); //(512 - quad.GetY()) - length;

        const Color color = GREEN;

        DrawRectangle(x, y, length, length, color);
    }

    EndTextureMode();


}


void DebugRenderer::Render() {
    DrawTextureRec(
    debugRegionTexture.texture, 
    (Rectangle) {
        .x = 0,
        .y = 0,
        .width = WINDOW_W,
        .height = -WINDOW_H,
    }, (Vector2) {0, 0}, WHITE);
    DrawTextureRec(
    debugTexture.texture, 
    (Rectangle) {
        .x = 0,
        .y = 0,
        .width = WINDOW_W,
        .height = -WINDOW_H,
    }, (Vector2) {0, 0}, WHITE);
}


DebugRenderer::~DebugRenderer() {
    UnloadRenderTexture(debugTexture);
}