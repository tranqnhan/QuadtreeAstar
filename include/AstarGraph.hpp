#pragma once

#include "Quadtree.hpp"
#include <cmath>
#include <vector>

class AstarNode {
public:
    AstarNode(int x, int y, int edgeIndex) 
    : x(x), y(y), edgeIndex(edgeIndex) {
        numEdges = 0;
    }

    int GetX() const {
        return x;
    }

    int GetY() const {
        return y;
    }

    int GetNumEdges() const {
        return numEdges;
    }

    int GetEdgeIndex() const {
        return edgeIndex;
    }

    void IncrementNumEdges() {
        numEdges++;
    }

private:
    int x, y;
    int numEdges;
    int edgeIndex;

};


class AstarEdge {
public:
    AstarEdge(int nodeIdB, float dist) 
    : nodeIdB(nodeIdB), dist(dist) {}

    int GetNodeIdB() const {
        return nodeIdB;
    }

    float GetDist() const {
        return dist;
    }

private:
    int nodeIdB;
    float dist;
};


class AstarGraph {
public:

    AstarGraph(const Quadtree &quadtree);

    const std::vector<AstarNode>& GetNodes() const {
        return nodes;
    }

    const std::vector<AstarEdge>& GetEdges() const {
        return edges;
    }

    void Clear();


private:
    std::vector<AstarNode> nodes;
    std::vector<AstarEdge> edges;

    void AddNode(int x, int y,int edgeIndex);
    void AddEdge(int nodeIdA, int nodeIdB);

};