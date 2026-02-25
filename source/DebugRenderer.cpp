#include <cstdio>
#include <vector>

#include <raylib.h>

#include "AstarGraph.hpp"
#include "Program.hpp"
#include "Quadtree.hpp"
#include "DebugRenderer.hpp"


void DebugRenderer::Init() {
    debugTexture = LoadRenderTexture(WINDOW_W, WINDOW_H);
    debugPathTexture = LoadRenderTexture(WINDOW_W, WINDOW_H);

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


void DebugRenderer::UpdatePath(const std::vector<int>& path) {

    BeginTextureMode(debugPathTexture);
    ClearBackground(BLANK);

    for (int i = 2; i < path.size(); i += 2) {
        DrawLineEx(Vector2{(float)path[i - 2], (float)path[i - 1]}, Vector2{(float)path[i], (float)path[i + 1]}, 5, DARKGREEN);
    }

    EndTextureMode();


}


void DebugRenderer::Render() {
    DrawTextureRec(
    debugTexture.texture, 
    (Rectangle) {
        .x = 0,
        .y = 0,
        .width = WINDOW_W,
        .height = -WINDOW_H,
    }, (Vector2) {0, 0}, WHITE);
    DrawTextureRec(
    debugPathTexture.texture, 
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