#pragma once

#include "AstarGraph.hpp"

class AstarSearch {
public:
    std::vector<int> GetPath(
      const Quadtree& quadtree, const AstarGraph& graph,
      int fromX, int fromY, int toX, int toY);


};