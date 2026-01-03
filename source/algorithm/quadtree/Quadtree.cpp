#include <iostream>

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


REGION Quadtree::SubdivideCondition(const Quadrant& leaf, const std::vector<bool>& grid, const int gridWidth) {
    int x = leaf.GetX();
    int y = leaf.GetY();
    int width = leaf.GetWidth();
    int height = leaf.GetHeight();


    bool isValid = grid[y * gridWidth + x];
    std::cout << x << " " << y << " " << width << " h: " << height << isValid << "\n";

    for (int i = y; i < y + height; ++i) {
        for (int j = x; j < x + width; ++j) {
            if (grid[i * gridWidth + j] != isValid) {
                std::cout << j << " " << i << " undecided \n";
                return REGION::UNDECIDED;
            }
        }
    }
    std::cout << isValid << " decided \n";

    return isValid ? REGION::VALID : REGION::BLOCK;
}


void Quadtree::Build(const std::vector<bool>& grid, const int gridWidth, const int gridHeight) {
    std::vector<Quadrant> stack;
    int x, y;
    int width, height;
    int midX, midY;
    uint64_t locationCode;
    int level;

    stack.emplace_back(0, 0, gridWidth, gridHeight, 0, 0);

    while(stack.size() > 0) {
        Quadrant leaf = stack[stack.size() - 1];
        stack.pop_back();
        
        x = leaf.GetX();
        y = leaf.GetY();
        width = leaf.GetWidth();
        height = leaf.GetHeight();

        this->SubdivideRect(midX, midY, width, height, x, y);

        REGION result = this->SubdivideCondition(leaf, grid, gridWidth);

        if (result == REGION::UNDECIDED && leaf.GetLevel() < this->resolution) {
            level = leaf.GetLevel() + 1;
            locationCode = std::pow(10, this->resolution - (level - 1));

            stack.emplace_back(x, midY, width, height, 0, level);
            stack.emplace_back(midX, midY, width, height, locationCode, level);
            stack.emplace_back(midX, y, width, height, locationCode * 2, level);
            stack.emplace_back(x, y, width, height, locationCode * 3, level);
        } else {
            this->leafs.emplace_back(std::move(leaf));
            this->leafsValid.emplace_back(result == REGION::VALID);
            this->leafIndex[leaf.GetCode()] = this->leafs.size() - 1;
        }
    }
    
}