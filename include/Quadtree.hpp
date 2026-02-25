#pragma once

#include <cstdint>
#include <ankerl/unordered_dense.h>
#include <vector>

#include "BinaryMath.hpp"
#include "GridEnvironment.hpp"

struct QuadrantIdentifier {
    uint64_t locationCode;
    int level;
    int dir[4];

    QuadrantIdentifier(uint64_t locationCode, int level) : 
        locationCode(locationCode),
        level(level) 
    {}

    QuadrantIdentifier(uint64_t locationCode, int level, int n, int s, int w, int e) : 
        locationCode(locationCode),
        level(level) 
    {
        dir[0] = s;
        dir[1] = n;
        dir[2] = w;
        dir[3] = e;
    }
};

/**
 * The leafs of the quadtree
 */
class Quadrant {
public:
    Quadrant(uint64_t locationCode, int level, bool isValid) :
        locationCode(locationCode),
        level(level),
        isValid(isValid) 
    {}

    int GetX() const {
        return BinaryMath::Deinterleave(this->locationCode);
    }

    int GetY() const {
        return BinaryMath::Deinterleave(this->locationCode >> 1);
    }

    int GetLevel() const {
        return level;
    }

    uint64_t GetCode() const {
        return locationCode;
    }

    bool IsValid() const {
        return isValid;
    }

private:
    int level;
    uint64_t locationCode;
    bool isValid;
};


class Quadtree {
public:
    Quadtree();

    void Init(int resolution);

    void Build(const GridEnvironment& grid, int maxLevel);
    
    const std::vector<Quadrant>& GetLeafs() const {
        return leafs;
    }
    
    int GetResolution() const {
        return resolution;
    }

    const std::vector<std::vector<int>>& GetGraph() const {
        return quadtreeGraph;
    }

    // Returns the index of the quadrant
    int QueryValidRegion(uint32_t x, uint32_t y) const;


private:

    uint64_t tx;
    uint64_t ty;

    uint64_t dI[4]; // SOUTH, NORTH, WEST, EAST

    int resolution;
    int maxLevel;

    std::vector<Quadrant> leafs;

    std::vector<std::vector<int>> quadtreeGraph;
    ankerl::unordered_dense::map<uint64_t, int> leafIndex;
        
    void SubdivideRegionLarge(uint64_t fromIndex, uint64_t lowerBound, bool oldValid, int maxLevel);
    void SubdivideRegionSmall(uint64_t fromIndex, uint64_t upperBound, bool oldValid, int maxLevel);
    
    void BuildRegion(const GridEnvironment& grid, int maxLevel);
    void BuildLevelDifferences(ankerl::unordered_dense::map<uint64_t, QuadrantIdentifier> &mapIdentifiers, int maxLevel);
    void BuildGraph(const ankerl::unordered_dense::map<uint64_t, QuadrantIdentifier> &mapIdentifiers, int maxLevel);

    uint64_t DialatedIntegerAdd(uint64_t locationCode, uint64_t direction) const;
    uint64_t GetAdjacentQuadrant(uint64_t locationCode, int direction, int shift) const; 
    int GetChildLevelDiff(const QuadrantIdentifier& parent, int dir) const;
    
    void IncrementAdjacentQuad(
        ankerl::unordered_dense::map<uint64_t, QuadrantIdentifier>& mapIdentifiers,
        const QuadrantIdentifier& quad,
        int direction,
        int adjacentShift
    );

    


};
