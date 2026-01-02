#include <cstdint>
#include <cmath>
#include <raylib.h>
#include <sys/types.h>
#include <vector>

#include "Quadtree.hpp"

// Quadtree Leaf
Quadrant::Quadrant(int x, int y, int width, int height, uint64_t locationCode, int level) {
    this->locationCode = locationCode;
    this->level = level;

    bbox[0] = x;
    bbox[1] = y;
    bbox[2] = width;
    bbox[3] = height;
}

// Quadtree
Quadtree::Quadtree(int resolution) {
    this->resolution = resolution > 32 ? 32 : resolution; // caps the resolution to 32
    int push = (32 - resolution) * 2;
    
    // tx = 01...0101 “01” repeated r times
    // ty = 10...1010 “10” repeated r times
    this->ty = 6148914691236517205u >> push;
    this->tx = 12297829382473034410u >> push;
}


void Quadtree::SubdivideRect(int& midX, int& midY, int& width, int &height, int x, int y) {
    width /= 2;
    height /= 2;
    midX = width + x;
    midY = height + y;
}


uint64_t Quadtree::LocationAdd(uint64_t locationCode, uint64_t direction) {
    return 
        (((locationCode | this->ty) + (direction & this->tx)) & this->tx) | 
        (((locationCode | this->tx) + (direction & this->ty)) & this->ty);
}


uint64_t Quadtree::GetAdjacentQuadrant(uint64_t locationCode, uint64_t direction, int level) {
    return this->LocationAdd(locationCode, direction << (2 * (resolution - level)));
}


void Quadtree::Build(int x, int y, int width, int height) {
    std::vector<Quadrant> stack;

    stack.emplace_back(x, y, width, height, 0, 0);

    int midX, midY;

    uint64_t locationCode;
    int level;
    
    while(stack.size() > 0) {
        Quadrant leaf = stack[stack.size() - 1];
        stack.pop_back();
        
        x = leaf.GetX();
        y = leaf.GetY();
        width = leaf.GetWidth();
        height = leaf.GetHeight();

        this->SubdivideRect(midX, midY, width, height, x, y);

        if (this->SubdivideCondition(leaf)) {
            level = leaf.GetLevel() + 1;
            locationCode = std::pow(10, this->resolution - (level - 1));

            stack.emplace_back(x, midY, width, height, 0, level);
            stack.emplace_back(midX, midY, width, height, locationCode, level);
            stack.emplace_back(midX, y, width, height, locationCode * 2, level);
            stack.emplace_back(x, y, width, height, locationCode * 3, level);
        } else {
            this->leafs.emplace_back(std::move(leaf));
            this->leafIndex[leaf.GetCode()] = this->leafs.size() - 1;
        }
    }
    
}