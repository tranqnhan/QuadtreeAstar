#include <cstdint>
#include <cstdio>
#include <cmath>

#include <queue>
#include <unordered_map>
#include <ankerl/unordered_dense.h>
#include <vector>

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
    this->resolution = resolution > 19 ? 19 : resolution;
    int push = (18 - resolution) * 2;
    
    // tx = 01...0101 “01” repeated r times
    // ty = 10...1010 “10” repeated r times
    this->ty = 6148914691236517205u >> push;
    this->tx = 12297829382473034410u >> push;
}


uint64_t Quadtree::Pow10(int n) {
    static uint64_t table[] = {
        1LL, 10LL, 100LL, 1000LL, 10000LL, 100000LL, 1000000LL, 
        10000000LL, 100000000LL, 1000000000LL, 10000000000LL, 
        100000000000LL, 1000000000000LL, 10000000000000LL, 
        100000000000000LL, 1000000000000000LL, 10000000000000000LL, 
        100000000000000000LL, 1000000000000000000LL, 10000000000000000000LLU
    };

    if (n >= 0 && n < 20) {
        return table[n];
    }

    return 1; 
}

void Quadtree::SubdivideRect(int& midX, int& midY, int& width, int &height, int x, int y) {
    width /= 2;
    height /= 2;
    midX = width + x;
    midY = height + y;
}


uint64_t Quadtree::LocationAdd(uint64_t locationCode, uint64_t direction) const {
    return 
        (((locationCode | this->ty) + (direction & this->tx)) & this->tx) | 
        (((locationCode | this->tx) + (direction & this->ty)) & this->ty);
}


//uint64_t Quadtree::GetAdjacentQuadrant(uint64_t locationCode, uint64_t direction, int levelDiff, int level) const {
//    if (levelDiff < 0) {
//        int shiftBy = 2 * (resolution - level - levelDiff);
//        return this->LocationAdd((locationCode >> levelDiff) << levelDiff, direction << levelDiff);
//    } else {
//        return this->LocationAdd(locationCode, direction << (2 * (resolution - level)));
//    }
//}


uint64_t Quadtree::GetAdjacentQuadrant(uint64_t locationCode, uint64_t direction, int shift) const {
    return this->LocationAdd(locationCode, direction << shift);
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


Region Quadtree::BuildRegion(const GridEnvironment& grid, ankerl::unordered_dense::map<uint64_t, int>& leafCodes, const int x, const int y, const int width, const int height, uint64_t locationCode, int level) {
    if (width == 1) {
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

    regionStatus[0] = this->BuildRegion(grid, leafCodes, pos[0], pos[1], childWidth, childHeight, locationCode * 10, level + 1);
    regionStatus[1] = this->BuildRegion(grid, leafCodes, pos[2], pos[3], childWidth, childHeight, locationCode * 10 + 1, level + 1);
    regionStatus[2] = this->BuildRegion(grid, leafCodes, pos[4], pos[5], childWidth, childHeight, locationCode * 10 + 2, level + 1);
    regionStatus[3] = this->BuildRegion(grid, leafCodes, pos[6], pos[7], childWidth, childHeight, locationCode * 10 + 3, level + 1);
    
    if (regionStatus[0] == Region::Mixed ||
        regionStatus[0] != regionStatus[1] ||
        regionStatus[0] != regionStatus[2] ||
        regionStatus[0] != regionStatus[3]) {
        for (int i = 0; i < numRegions; ++i) {
            if (regionStatus[i] == Mixed) continue;

            uint64_t code = (locationCode * 10 + i) * this->Pow10(this->resolution - level);
            
            leafCodes[code] = level;

            if (regionStatus[i] == Region::Valid) {
                this->leafs.emplace_back(
                    pos[2 * i], pos[2 * i + 1], 
                    childWidth, 
                    childHeight,
                    code,
                    level);
            }
        }


        return Region::Mixed;
    }

    return regionStatus[0];
}


void Quadtree::Build(const GridEnvironment& grid) {

    // Get leafs
    ankerl::unordered_dense::map<uint64_t, int> leafCodes; // for adjacent level differences
    
    Region region = Quadtree::BuildRegion(grid, leafCodes, 0, 0, grid.GetWidth(), grid.GetHeight(), 0, 1);
    
    if (region == Region::Valid) {
        this->leafs.emplace_back(0, 0, grid.GetWidth(), grid.GetHeight(), 0, 0);
        return;
    }


    
    // Adjacent level differences
    std::queue<QuadrantIdentifier> queues;
    std::queue<QuadrantLevelDiff> directions;

    queues.emplace(0, 0);
    directions.emplace(~0, ~0, ~0, ~0);
    
    while (queues.size() > 0) {
        
        QuadrantIdentifier currentId = queues.front();


        if (leafCodes.find(currentId.locationCode) == leafCodes.end() 
            || leafCodes[currentId.locationCode] != currentId.level) {
                        
            for (int k = 0; k < 4; ++k) {
                uint64_t childLocation = currentId.locationCode + k * this->Pow10(this->resolution - currentId.level - 1);
                queues.emplace(childLocation, currentId.level + 1);

            }

            // TODO: 
        }

        queues.pop();
    } 
    


}

/**
    Interleaving Algorithm from Daniel Lemire's blog
    How fast can you bit-interleave 32-bit integers?
    and
    https://stackoverflow.com/questions/4909263/how-to-efficiently-de-interleave-bits-inverse-morton
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


