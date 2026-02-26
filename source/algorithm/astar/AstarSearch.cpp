#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>
#include <limits>

#include "ankerl/unordered_dense.h"

#include "AstarSearch.hpp"
#include "AstarGraph.hpp"
#include "Quadtree.hpp"
#include "Heap.hpp"



// Returns a list of x y coords
std::vector<int> AstarSearch::GetPath(const Quadtree& quadtree, const AstarGraph& graph, int fromX, int fromY, int toX, int toY) {
    std::vector<int> path; // xyxyxy...

    int fromRegionIndex = quadtree.QueryValidRegion((uint32_t)fromX, (uint32_t)fromY);
    int toRegionIndex = quadtree.QueryValidRegion((uint32_t)toX, (uint32_t)toY);


    if (fromRegionIndex == -1 || toRegionIndex == -1) {
        return path;
    }

    // They are in the same region.
    if (fromRegionIndex == toRegionIndex) {
        path.emplace_back(fromX);
        path.emplace_back(fromY);
        path.emplace_back(toX);
        path.emplace_back(toY);
        return path;
    }

    const std::vector<AstarNode>& nodes = graph.GetNodes();
    const std::vector<AstarEdge>& edges = graph.GetEdges();

    std::vector<int> parent(nodes.size(), -1);

    std::vector<float> gScores(nodes.size(), std::numeric_limits<float>::max());

    Heap<float> openSet = Heap<float>(
        [](const float& a, const float& b) -> bool {
            return a > b;
        }
    );

    ankerl::unordered_dense::set<int> closeSet;

    openSet.Push(fromRegionIndex, fromRegionIndex);

    gScores[fromRegionIndex] = 0;

    bool isPathFound = false;

    while(openSet.GetSize() > 0) {

        // Find node with smallest fScore
        const int currentNodeIndex = openSet.TopItemID();

        openSet.Pop();

        if (currentNodeIndex == toRegionIndex) {
            isPathFound = true;
            break;
        } 

        closeSet.emplace(currentNodeIndex);

        // Expand neighbors
        const AstarNode& currentNode = nodes[currentNodeIndex];

        for (int i = currentNode.GetEdgeIndex(); i < currentNode.GetEdgeIndex() + currentNode.GetNumEdges(); ++i) {
            const int nextNodeIndex = edges[i].GetNodeIdB();
            const float nextNodeDist = edges[i].GetDist();

            if (closeSet.find(nextNodeIndex) != closeSet.end()) {
                continue;
            }
            
            const float gScore = gScores[currentNodeIndex] + nextNodeDist;

            if (gScore < gScores[nextNodeIndex]) {
                parent[nextNodeIndex] = currentNodeIndex;
                gScores[nextNodeIndex] = gScore;

                const int nextNodeX = nodes[nextNodeIndex].GetX();
                const int nextNodeY = nodes[nextNodeIndex].GetY();
                const float dX = nextNodeX - toX;
                const float dY = nextNodeY - toY;
                const float hScore = std::sqrt(dX * dX + dY * dY);
                
                const float fScore = gScore + hScore;

                if (openSet.Push(fScore, nextNodeIndex)) {
                    parent[nextNodeIndex] = currentNodeIndex;
                }
            }
            
        }

    }

    // Reconstruct path
    if (isPathFound) {
        path.emplace_back(toY);
        path.emplace_back(toX);

        int currentNodeIndex = toRegionIndex;

        do {
            const int y = nodes[currentNodeIndex].GetY();
            const int x = nodes[currentNodeIndex].GetX();
            path.emplace_back(y);
            path.emplace_back(x);
            currentNodeIndex = parent[currentNodeIndex];
        } while (currentNodeIndex != -1);

        path.emplace_back(fromY);
        path.emplace_back(fromX);

        std::reverse(path.begin(), path.end());
    }

    return path;
};