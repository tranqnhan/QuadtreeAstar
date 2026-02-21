#pragma once

#include <cmath>
#include <vector>
class Node {
public:
    Node(int x, int y, int edgeIndex) 
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


class Edge {
public:
    Edge(int nodeIdB, float dist) 
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


class Graph {
public:

    void AddNode(int x, int y,int edgeIndex) {
        nodes.emplace_back(x, y, edgeIndex);
    }

    void AddEdge(int nodeIdA, int nodeIdB) {
        if (nodeIdA >= nodes.size() && nodeIdB >= nodes.size()) return;

        const Node& nodeA = nodes[nodeIdA];
        const Node& nodeB = nodes[nodeIdB];

        const float dx = nodeB.GetX() - nodeA.GetX();
        const float dy = nodeB.GetY() - nodeA.GetY();
        const float dist = std::sqrt(dx * dx + dy * dy);
        
        edges[nodeA.GetEdgeIndex() + nodeA.GetNumEdges()] = Edge(nodeIdB, dist);
    }

    void Clear() {
        nodes.clear();
        edges.clear();
    }


private:
    std::vector<Node> nodes;
    std::vector<Edge> edges;

};