#pragma once

#include "IntVector2.hpp"
#include <cstdint>

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
class QuadtreeLeaf {
public:
    QuadtreeLeaf(const IntVector2& pos, const IntVector2& dim, uint64_t locationCode);
    QuadtreeLeaf(int x, int y, int width, int height, uint64_t locationCode);

    int getX() const {
        return bbox[0];
    };

    int getY() const {
        return bbox[1];
    };

    int getWidth() const {
        return bbox[2];
    };

    int getHeight() const {
        return bbox[3];
    };


private:
    int bbox[4];
    int locationCode;

    void Init(int x, int y, int width, int height, uint64_t locationCode);

};


class Quadtree {
public:
    Quadtree(int resolution);

    IntVector2 midpoint(IntVector2 pos1, IntVector2 pos2);
    uint64_t locationAdd(uint64_t locationCode, uint64_t direction);
    uint64_t getAdjacentQuadrant(uint64_t locationCode, uint64_t direction, int level);

private:

    uint64_t tx;
    uint64_t ty;

    int resolution;

};
