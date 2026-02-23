#include <vector>

#include "AstarGraph.hpp"
#include "Quadtree.hpp"


void AstarGraph::Build(const Quadtree &quadtree) {
    nodes.clear();
    edges.clear();
    
    const std::vector<std::vector<int>> &quadtreeGraph = quadtree.GetGraph();
    const std::vector<Quadrant> &quadtreeLeafs = quadtree.GetLeafs();

    // Adding Nodes
    int halfLength, x, y, numEdges;
    
    halfLength = (1 << (quadtree.GetResolution() - quadtreeLeafs[0].GetLevel())) / 2;
    x = quadtreeLeafs[0].GetX() + halfLength; 
    y = quadtreeLeafs[0].GetY() + halfLength; 
    this->AddNode(x, y, 0);
    numEdges += quadtreeGraph[0].size();

    for (int i = 1; i < quadtreeGraph.size(); ++i) {
        halfLength = (1 << (quadtree.GetResolution() - quadtreeLeafs[i].GetLevel())) / 2;
        x = quadtreeLeafs[i].GetX() + halfLength; 
        y = quadtreeLeafs[i].GetY() + halfLength;
        this->AddNode(x, y, numEdges);
        numEdges += quadtreeGraph[i].size();
    }

    // Adding Edges
    edges.resize(numEdges);

    for (int i = 0; i < quadtreeGraph.size(); ++i) {
        for (int j = 0; j < quadtreeGraph[i].size(); ++j) {
            this->AddEdge(i, quadtreeGraph[i][j]);
        }
    }
}


void AstarGraph::AddNode(int x, int y,int edgeIndex) {
    nodes.emplace_back(x, y, edgeIndex);
}


void AstarGraph::AddEdge(int nodeIdA, int nodeIdB) {
    if (nodeIdA >= nodes.size() && nodeIdB >= nodes.size()) {
        std::printf("ERROR\n");
        return;
    }
    AstarNode &nodeA = nodes[nodeIdA];
    AstarNode &nodeB = nodes[nodeIdB];

    const float dx = nodeB.GetX() - nodeA.GetX();
    const float dy = nodeB.GetY() - nodeA.GetY();
    const float dist = std::sqrt(dx * dx + dy * dy);
    
    edges[nodeA.GetEdgeIndex() + nodeA.GetNumEdges()] = AstarEdge(nodeIdB, dist);
    nodeA.IncrementNumEdges();
}


void AstarGraph::Clear() {
    nodes.clear();
    edges.clear();
}
