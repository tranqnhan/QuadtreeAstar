
#include <cstdint>
#include <raylib.h>
#include <sys/types.h>
#include <vector>

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


// Check if there is a border of invalids
bool Quadtree::BorderCheck(const GridEnvironment& grid, const int x, const int y, const int width, const int height) {
    for (int i = x; i < x + width; ++i) {
        if (grid.IsValid(y * grid.GetWidth() + i)) {
            return false;
        }
    }

    for (int i = x; i < x + width; ++i) {
        if (grid.IsValid((y + height - 1) * grid.GetWidth() + i)) {
            return false;
        }
    }

    for (int i = y; i < y + height; ++i) {
        if (grid.IsValid(i * grid.GetWidth() + x)) {
            return false;
        }
    }

    for (int i = y; i < y + height; ++i) {
        if (grid.IsValid(i * grid.GetWidth() + (x + width - 1))) {
            return false;
        }
    }


    return true;
}


Region Quadtree::BuildRegion(const GridEnvironment& grid, const int x, const int y, const int width, const int height) {
    if (width <= 1) {
        return grid.IsValid(y * grid.GetWidth() + x) ? Region::Block : Region::Valid;
    }

    if (this->BorderCheck(grid, x, y, width, height)) {
        // TODO: check if the region is covering the start / end
        return Region::Block;
    }

    int midX, midY;
    int childWidth = width;
    int childHeight = height;
    this->SubdivideRect(midX, midY, childWidth, childHeight, x, y);

    constexpr int numRegions = 4;

    Region regionStatus[numRegions];
    const int pos[8] = {x, midY, midX, midY, midX, y, x, y};

    regionStatus[0] = this->BuildRegion(grid, pos[0], pos[1], childWidth, childHeight);
    regionStatus[1] = this->BuildRegion(grid, pos[2], pos[3], childWidth, childHeight);
    regionStatus[2] = this->BuildRegion(grid, pos[4], pos[5], childWidth, childHeight);
    regionStatus[3] = this->BuildRegion(grid, pos[6], pos[7], childWidth, childHeight);

    if (regionStatus[0] != regionStatus[1] ||
        regionStatus[0] != regionStatus[2] ||
        regionStatus[0] != regionStatus[3] ||
        regionStatus[0] == Region::Mixed) {
        for (int i = 0; i < numRegions; ++i) {
            if (regionStatus[i] != Region::Mixed) {
                // TODO: add locationCode and level and color
                this->leafs.emplace_back(pos[2 * i], pos[2 * i + 1], childWidth, childHeight, 0, 0);
            }
        }
        return Region::Mixed;
    }

    return regionStatus[0];
}


void Quadtree::Build(const GridEnvironment& grid) {
    Region region = Quadtree::BuildRegion(grid, 0, 0, grid.GetWidth(), grid.GetHeight());
    if (region != Region::Mixed) {
        this->leafs.emplace_back(0, 0, grid.GetWidth(), grid.GetHeight(), 0, 0);
    }
}

// void Quadtree::Build(const std::vector<bool>& grid, const int gridWidth, const int gridHeight) {
//     std::vector<Quadrant> stack;
//     int x, y;
//     int width, height;
//     uint64_t locationCode;
//     int level;
//     int midX, midY;
// 
//     stack.emplace_back(0, 0, gridWidth, gridHeight, 0, 0);
// 
//     while(stack.size() > 0) {
//         Quadrant leaf = stack[stack.size() - 1];
//         stack.pop_back();
//         
//         x = leaf.GetX();
//         y = leaf.GetY();
//         width = leaf.GetWidth();
//         height = leaf.GetHeight();
// 
//         this->SubdivideRect(midX, midY, width, height, x, y);
// 
//         int result = this->CheckRegion(leaf, grid, gridWidth);
// 
//         if (leaf.GetLevel() < this->resolution) {
//             level = leaf.GetLevel() + 1;
//             locationCode = std::pow(10, this->resolution - (level - 1));
// 
//             stack.emplace_back(x, midY, width, height, 0, level);
//             stack.emplace_back(midX, midY, width, height, locationCode, level);
//             stack.emplace_back(midX, y, width, height, locationCode * 2, level);
//             stack.emplace_back(x, y, width, height, locationCode * 3, level);
//         } else {
//             this->leafs.emplace_back(std::move(leaf));
//             this->leafsValid.emplace_back(result == REGION::VALID);
//             this->leafIndex[leaf.GetCode()] = this->leafs.size() - 1;
//         }
//     }
//     
// }

uint64_t Quadtree::Interleave(uint32_t x, uint32_t y) {
    uint64_t val = 0;
    constexpr uint32_t mask = (1 << 31);
    for (int i = 0; i < 32; ++i) {
        val = (val << 1) + ((x & mask) != 0);
        val = (val << 1) + ((y & mask) != 0);
        x <<= 1; y <<= 1;
    }
    return val;
}
