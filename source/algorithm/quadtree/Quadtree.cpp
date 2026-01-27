
#include <array>
#include <cstddef>
#include <cstdint>
#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <stdio.h>

#include <raylib.h>

#include "Quadtree.hpp"
#include "GridEnvironment.hpp"

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


// Returns false if there exists an invalid spot in the region
bool Quadtree::ScanCheck(const GridEnvironment& grid, const int x, const int y, const int width, const int height) {
    const int xbound = x + width;
    const int ybound = y + height;

    for (int i = x; i < xbound; ++i) {    
        for (int j = y; j < ybound; ++j) {
            if (!grid.IsValid(j * grid.GetWidth() + i)) {
                return false;
            }
        }
    }

    return true;
}


// Check if there is a border of invalids
bool Quadtree::BorderCheck(const GridEnvironment& grid, const int x, const int y, const int width, const int height) {
    const int xbound = x + width;
    const int ybound = y + height;

    for (int i = x; i < xbound; ++i) {
        if (grid.IsValid(y * grid.GetWidth() + i)) {
            return false;
        }
    }

    for (int i = x; i < xbound; ++i) {
        if (grid.IsValid((ybound - 1) * grid.GetWidth() + i)) {
            return false;
        }
    }

    for (int i = y; i < ybound; ++i) {
        if (grid.IsValid(i * grid.GetWidth() + x)) {
            return false;
        }
    }

    for (int i = y; i < ybound; ++i) {
        if (grid.IsValid(i * grid.GetWidth() + (xbound - 1))) {
            return false;
        }
    }


    return true;
}


Region Quadtree::BuildRegion(const GridEnvironment& grid, const int x, const int y, const int width, const int height, uint64_t locationCode, int level) {
    if (width <= 1) {
        return grid.IsValid(y * grid.GetWidth() + x) ? Region::Valid : Region::Block;
    }

    if (this->BorderCheck(grid, x, y, width, height)) {
        // TODO: check if the region is covering the start / end
        return Region::Block;
    }

    if (level >= this->resolution) {
        return this->ScanCheck(grid, x, y, width, height) ? Region::Valid : Region::Block;
    }

    int midX, midY;
    int childWidth = width;
    int childHeight = height;
    this->SubdivideRect(midX, midY, childWidth, childHeight, x, y);

    constexpr int numRegions = 4;

    Region regionStatus[numRegions];
    const int pos[8] = {x, midY, midX, midY, x, y, midX, y};

    regionStatus[0] = this->BuildRegion(grid, pos[0], pos[1], childWidth, childHeight, locationCode * 10, level + 1);
    regionStatus[1] = this->BuildRegion(grid, pos[2], pos[3], childWidth, childHeight, locationCode * 10 + 1, level + 1);
    regionStatus[2] = this->BuildRegion(grid, pos[4], pos[5], childWidth, childHeight, locationCode * 10 + 2, level + 1);
    regionStatus[3] = this->BuildRegion(grid, pos[6], pos[7], childWidth, childHeight, locationCode * 10 + 3, level + 1);
    
    if (regionStatus[0] == Region::Mixed ||
        regionStatus[0] != regionStatus[1] ||
        regionStatus[0] != regionStatus[2] ||
        regionStatus[0] != regionStatus[3]) {
        for (int i = 0; i < numRegions; ++i) {
            if (regionStatus[i] == Region::Valid) {
                // TODO: add locationCode and level and color
                this->leafs.emplace_back(
                    pos[2 * i], pos[2 * i + 1], 
                    childWidth, 
                    childHeight,
                    (locationCode * 10 + i) * std::pow(10, (this->resolution - level - 1)),
                    level);
            }
        }
        return Region::Mixed;
    }

    return regionStatus[0];
}


//void Quadtree::Build(const GridEnvironment& grid) {
//    Region region = Quadtree::BuildRegion(grid, 0, 0, grid.GetWidth(), grid.GetHeight(), 0, 0);
//    
//    if (region == Region::Valid) {
//        this->leafs.emplace_back(0, 0, grid.GetWidth(), grid.GetHeight(), 0, 0);
//    }
//    
//}



void Quadtree::Build(const GridEnvironment& grid) {
    const char numQuads = 4;
    const size_t size = grid.GetWidth() * grid.GetHeight();

    // TODO: replace with a calculated the bounds of this
    std::vector<bool> stack(size);
    std::vector<uint32_t> bounds(size * numQuads);
    
    int head = 0;
    
    uint64_t x, y;
    bool valid;

    for (uint64_t z = 0; z < size; z += numQuads) {
        for (char i = 0; i < numQuads; ++i) {
            this->Deinterleave(z + i, x, y);

            bounds[head * numQuads + 0] = x;
            bounds[head * numQuads + 1] = y;
            bounds[head * numQuads + 2] = 1;
            bounds[head * numQuads + 3] = 1;
            
            stack[head++] = grid.IsValid(y * grid.GetWidth() + x);
        }

        do {
            head -= numQuads;
            valid = true;
            for (char j = 0; j < numQuads; ++j) {
                if (!stack[head + j]) {
                    valid = false;

                    // TODO: Room for optimization here
                    for (char k = 0; k < numQuads; ++k) {
                        if (stack[head + k]) {
                            this->leafs.emplace_back(
                                bounds[(head + k) * numQuads + 0], 
                                bounds[(head + k) * numQuads + 1],
                                bounds[(head + k) * numQuads + 2],
                                bounds[(head + k) * numQuads + 3],
                                0, 0);
                        }
                    }

                    break; 
                }
            }

            bounds[head * numQuads + 2] *= 2;
            bounds[head * numQuads + 3] *= 2;

            stack[head++] = valid;
        } while (head >= 4 && bounds[(head - 1) * numQuads + 2] == bounds[(head - 4) * numQuads + 2]);
    }
}


/**
    Interleaving Algorithm from Daniel Lemire's blog
    How fast can you bit-interleave 32-bit integers?
*/

uint64_t Quadtree::InterleaveZero(uint32_t input) const {
    uint64_t word = input;
    word = (word ^ (word << 16)) & 0x0000ffff0000ffff;
    word = (word ^ (word << 8 )) & 0x00ff00ff00ff00ff;
    word = (word ^ (word << 4 )) & 0x0f0f0f0f0f0f0f0f;
    word = (word ^ (word << 2 )) & 0x3333333333333333;
    word = (word ^ (word << 1 )) & 0x5555555555555555;
    return word;
}


uint64_t Quadtree::Interleave(uint32_t x, uint32_t y) const {
    return this->InterleaveZero(x) | (this->InterleaveZero(y) << 1);
}

void Quadtree::Deinterleave(uint64_t z, uint64_t& x, uint64_t& y) const {
    x = z & 0x5555555555555555;
    x = (x | (x >> 1)) & 0x3333333333333333;
    x = (x | (x >> 2)) & 0x0f0f0f0f0f0f0f0f;
    x = (x | (x >> 4)) & 0x00ff00ff00ff00ff;
    x = (x | (x >> 8)) & 0x0000ffff0000ffff;

    y = (z >> 1) & 0x5555555555555555;
    y = (y | (y >> 1)) & 0x3333333333333333;
    y = (y | (y >> 2)) & 0x0f0f0f0f0f0f0f0f;
    y = (y | (y >> 4)) & 0x00ff00ff00ff00ff;
    y = (y | (y >> 8)) & 0x0000ffff0000ffff;
}


