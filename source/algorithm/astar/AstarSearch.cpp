#include "AstarSearch.hpp"
#include "AstarGraph.hpp"
#include "Quadtree.hpp"
#include "ankerl/unordered_dense.h"
#include <algorithm>
#include <cstdint>
#include <vector>


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

    std::vector<int> openNodeIndexes;
    ankerl::unordered_dense::set<int> closeNodeIndexes;

    const std::vector<AstarNode>& nodes = graph.GetNodes();
    const std::vector<AstarEdge>& edges = graph.GetEdges();

    std::vector<int> prevNodeIndexes(nodes.size(), -1);
    std::vector<float> prevNodeDists(nodes.size(), -1);

    openNodeIndexes.emplace_back(fromRegionIndex);

    bool isPathFound = false;

    while(openNodeIndexes.size() > 0) {
        const int currentNodeIndex = openNodeIndexes.back();
        openNodeIndexes.pop_back();

        if (currentNodeIndex == toRegionIndex) {
            isPathFound = true;
            break;
        } 

        closeNodeIndexes.emplace(currentNodeIndex);

        const AstarNode& currentNode = nodes[currentNodeIndex];

        for (int i = currentNode.GetEdgeIndex(); i < currentNode.GetEdgeIndex() + currentNode.GetNumEdges(); ++i) {
            const int nextNodeIndex = edges[i].GetNodeIdB();
            const float nextNodeDist = edges[i].GetDist();

            if (closeNodeIndexes.find(nextNodeIndex) != closeNodeIndexes.end()) {
                continue;
            }
            
            
            openNodeIndexes.emplace_back(nextNodeIndex);
            prevNodeIndexes[nextNodeIndex] = currentNodeIndex;
            prevNodeDists[nextNodeIndex] = nextNodeDist;
        }

    }

    if (isPathFound) {
        path.emplace_back(toY);
        path.emplace_back(toX);

        int currentNodeIndex = toRegionIndex;

        do {
            const int y = nodes[currentNodeIndex].GetY();
            const int x = nodes[currentNodeIndex].GetX();
            path.emplace_back(y);
            path.emplace_back(x);
            currentNodeIndex = prevNodeIndexes[currentNodeIndex];
        } while (currentNodeIndex != -1);

        path.emplace_back(fromY);
        path.emplace_back(fromX);

        std::reverse(path.begin(), path.end());
    }

    return path;
};