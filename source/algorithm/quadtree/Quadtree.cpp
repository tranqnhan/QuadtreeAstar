#include <cstdint>
#include <raylib.h>

#include "Quadtree.hpp"
#include "IntVector2.hpp"


// Quadtree Leaf

void QuadtreeLeaf::Init(int x, int y, int width, int height, uint64_t locationCode) {
    bbox[0] = x;
    bbox[1] = y;
    bbox[2] = width;
    bbox[3] = height;

    this->locationCode = locationCode;
}

QuadtreeLeaf::QuadtreeLeaf(const IntVector2& pos, const IntVector2& dim, uint64_t locationCode) {
    this->Init(pos.x, pos.y, dim.x, dim.y, locationCode);
}

QuadtreeLeaf::QuadtreeLeaf(int x, int y, int width, int height, uint64_t locationCode) {
    this->Init(x, y, width, height, locationCode);
}


// Quadtree

Quadtree::Quadtree(int resolution) {
    this->resolution = resolution > 64 ? 64 : resolution; // caps the resolution to 64
    int push = (64 - resolution) * 2;
    
    // tx = 01...0101 “01” repeated r times
    // ty = 10...1010 “10” repeated r times
    this->ty = 6148914691236517205u >> push;
    this->tx = 12297829382473034410u >> push;
}

IntVector2 Quadtree::midpoint(IntVector2 pos1, IntVector2 pos2) {
    return IntVector2 { .x=(pos1.x + pos2.y) / 2, .y=(pos1.y + pos2.y) / 2};
}

uint64_t Quadtree::locationAdd(uint64_t locationCode, uint64_t direction) {
    return 
        (((locationCode | this->ty) + (direction & this->tx)) & this->tx) | 
        (((locationCode | this->tx) + (direction & this->ty)) & this->ty);
}

uint64_t Quadtree::getAdjacentQuadrant(uint64_t locationCode, uint64_t direction, int level) {
    return locationAdd(locationCode, direction << (2 * (resolution - level)));
}