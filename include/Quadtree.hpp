#pragma once

#include <cstdint>
#include <unordered_map>
#include <ankerl/unordered_dense.h>
#include <vector>

#include "GridEnvironment.hpp"

/**
 * Implementation of Linear Quadtree with Level Differences from
 * A Constant-Time Algorithm for Finding Neighbors in Quadtrees
 * by Kunio Aizawa and Shojiro Tanaka 
 */

#define DIR_N  0b000010u // north neighbor
#define DIR_E  0b000001u // east neighbor
#define DIR_W  0b010101u // west neighbor
#define DIR_S  0b101010u // south neighbor


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
    Quadrant(int x, int y, int width, int height, uint64_t locationCode, int level);

    int GetX() const {
        return bbox[0];
    };

    int GetY() const {
        return bbox[1];
    };

    int GetWidth() const {
        return bbox[2];
    };

    int GetHeight() const {
        return bbox[3];
    };

    int GetLevel() const {
        return level;
    }

    uint64_t GetCode() const {
        return locationCode;
    }

private:
    int bbox[4];
    int levelDifferences[4];    
    int level;
    uint64_t locationCode;
    
};


enum Region {
    Mixed,
    Valid,
    Block
};


class Quadtree {
public:
    Quadtree(int resolution);
    void Build(const GridEnvironment& grid);
    
    const std::vector<Quadrant>& GetLeafs() const {
        return leafs;
    }

        
    int GetResolution() const {
        return resolution;
    }

    const std::vector<std::vector<int>>& GetGraph() const {
        return graph;
    }

private:
    uint64_t tx;
    uint64_t ty;

    uint64_t directionsI[4]; // SOUTH, NORTH, WEST, EAST

    int resolution;
    int maxLevel;

    std::vector<Quadrant> leafs;
    ankerl::unordered_dense::map<uint64_t, int> leafIndex;

    std::vector<std::vector<int>> graph;
    
    bool ScanCheck(const GridEnvironment& grid, const int x, const int y, const int width, const int height);
    void SubdivideRect(int& midX, int& midY, int& width, int &height, int x, int y);
    uint64_t DialatedIntegerAdd(uint64_t locationCode, uint64_t direction) const;
    uint64_t GetAdjacentQuadrant(uint64_t locationCode, int direction, int shift) const; 
    Region BuildRegion(const GridEnvironment& grid, ankerl::unordered_dense::map<uint64_t, int>& leafCodes, const int x, const int y, const int width, const int height, uint64_t locationCode, int level);
    bool BorderCheck(const GridEnvironment& grid, const int x, const int y, const int width, const int height);
    uint64_t Interleave(uint32_t x, uint32_t y) const;
    uint64_t InterleaveZero(uint32_t input) const;
    void Deinterleave(uint64_t z, uint64_t& x, uint64_t& y) const;
    uint64_t Pow10(int n);
    int GetChildLevelDiff(const QuadrantIdentifier& parent, int dir) const;
    
    void IncrementAdjacentQuad(
        ankerl::unordered_dense::map<uint64_t, QuadrantIdentifier>& mapIdentifiers,
        const QuadrantIdentifier& quad,
        int direction,
        int adjacentShift
    );



};
