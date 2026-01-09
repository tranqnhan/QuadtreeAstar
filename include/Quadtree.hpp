#pragma once

#include "GridEnvironment.hpp"
#include <cstdint>
#include <unordered_map>
#include <vector>

/**
 * Implementation of Linear Quadtree with Level Differences from
 * A Constant-Time Algorithm for Finding Neighbors in Quadtrees
 * by Kunio Aizawa and Shojiro Tanaka 
 */

#define DIR_N  0x000010 // north neighbor
#define DIR_E  0x000001 // east neighbor
#define DIR_W  0x010101 // west neighbor
#define DIR_S  0x101010 // south neighbor
#define DIR_NE 0x000011 // northeast neighbor
#define DIR_NW 0x010111 // northwest neighbor
#define DIR_SW 0x111111 // southwest neighbor
#define DIR_SE 0x101011 // southeast neighbor


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

private:
    uint64_t tx;
    uint64_t ty;

    int resolution;

    std::vector<Quadrant> leafs;
    std::vector<bool> leafsValid;
    std::unordered_map<uint64_t, size_t> leafIndex;

    void SubdivideRect(int& midX, int& midY, int& width, int &height, int x, int y);
    uint64_t LocationAdd(uint64_t locationCode, uint64_t direction);
    uint64_t GetAdjacentQuadrant(uint64_t locationCode, uint64_t direction, int level);
    Region BuildRegion(const GridEnvironment& grid, const int x, const int y, const int width, const int height);
    uint64_t Interleave(uint32_t x, uint32_t y);
    bool BorderCheck(const GridEnvironment& grid, const int x, const int y, const int width, const int height);

};
