#include <cstdint>

#include <cstdio>
#include <queue>
#include <vector>

#include <ankerl/unordered_dense.h>
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
    this->resolution = resolution > 32 ? 32 : resolution;
    const int push = (32 - resolution) * 2;

    // SOUTH
    this->dI[0] = this->Interleave(0, (uint32_t)(~0) >> (push / 2));
    // NORTH
    this->dI[1] = 0b10;
    // WEST
    this->dI[2] = this->Interleave((uint32_t)(~0) >> (push / 2), 0);
    // EAST
    this->dI[3] = 0b01;

    // tx = 01...0101 “01” repeated r times
    // ty = 10...1010 “10” repeated r times
    this->tx = 0x5555555555555555 >> push;
    this->ty = 0xAAAAAAAAAAAAAAAA >> push;
    std::printf("push: %i\nres: %i\ntx %lb \nty %lb\n", push, resolution, this->tx, this->ty);
}



void Quadtree::SubdivideRect(int& midX, int& midY, int& width, int &height, int x, int y) {
    width /= 2;
    height /= 2;
    midX = width + x;
    midY = height + y;
}


uint64_t Quadtree::DialatedIntegerAdd(uint64_t locationCode, uint64_t direction) const {
    return 
        (((locationCode | this->ty) + (direction & this->tx)) & this->tx) | 
        (((locationCode | this->tx) + (direction & this->ty)) & this->ty);
}


void Quadtree::IncrementAdjacentQuad(
    ankerl::unordered_dense::map<uint64_t, QuadrantIdentifier>& mapIdentifiers,
    const QuadrantIdentifier& quad,
    int direction,
    int adjacentShift
) {

    if (quad.dir[direction] != 2) {
        uint64_t adjacentQuadCode = this->GetAdjacentQuadrant(quad.locationCode, direction, adjacentShift);
        auto adjacentQuad = mapIdentifiers.find(adjacentQuadCode);

        if (adjacentQuad != mapIdentifiers.end() && adjacentQuad->second.level == quad.level) {
            adjacentQuad->second.dir[direction ^ 1]++;
        }
    }
}


uint64_t Quadtree::GetAdjacentQuadrant(uint64_t locationCode, int direction, int shift) const {
    return this->DialatedIntegerAdd(locationCode, this->dI[direction] << shift);
}


int Quadtree::GetChildLevelDiff(const QuadrantIdentifier& parent, int d) const {
    if (parent.dir[d] == 2) {
        return 2;
    } 

    return parent.dir[d] - 1;
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


Region Quadtree::BuildRegion(const GridEnvironment& grid, ankerl::unordered_dense::map<uint64_t, int>& mapLevels, const int x, const int y, const int width, const int height, uint64_t locationCode, int level) {
    if (width == 1) {
        return grid.IsValid(y * grid.GetWidth() + x) ? Region::Valid : Region::Block;
    }

    if (this->BorderCheck(grid, x, y, width, height)) {
        // TODO: check if the region is covering the start / end
        return Region::Block;
    }

    if (level > this->resolution) {
        return this->ScanCheck(grid, x, y, width, height) ? Region::Valid : Region::Block;
    }

    int midX, midY;
    int childWidth = width;
    int childHeight = height;
    this->SubdivideRect(midX, midY, childWidth, childHeight, x, y);

    constexpr int numRegions = 4;

    Region regionStatus[numRegions];
    const int pos[8] = {x, midY, midX, midY, x, y, midX, y};

    regionStatus[0] = this->BuildRegion(grid, mapLevels, pos[0], pos[1], childWidth, childHeight, locationCode << 2, level + 1);
    regionStatus[1] = this->BuildRegion(grid, mapLevels, pos[2], pos[3], childWidth, childHeight, locationCode << 2 | 1, level + 1);
    regionStatus[2] = this->BuildRegion(grid, mapLevels, pos[4], pos[5], childWidth, childHeight, locationCode << 2 | 2, level + 1);
    regionStatus[3] = this->BuildRegion(grid, mapLevels, pos[6], pos[7], childWidth, childHeight, locationCode << 2 | 3, level + 1);
    
    if (regionStatus[0] == Region::Mixed ||
        regionStatus[0] != regionStatus[1] ||
        regionStatus[0] != regionStatus[2] ||
        regionStatus[0] != regionStatus[3]) {
        for (uint64_t i = 0; i < numRegions; ++i) {
            if (regionStatus[i] == Mixed) continue;

            uint64_t code = (locationCode << (2 * (this->resolution - level + 1))) | (i << (2 * (this->resolution - level)));
            
            mapLevels.emplace(code, level);

            if (regionStatus[i] == Region::Valid) {
                this->leafIndex.emplace(code, this->leafs.size());
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
    ankerl::unordered_dense::map<uint64_t, int> mapLevels; // for adjacent level differences
    
    Region region = Quadtree::BuildRegion(grid, mapLevels, 0, 0, grid.GetWidth(), grid.GetHeight(), 0, 1);
    
    if (region != Region::Mixed) {
        if (region == Region::Valid) {
            this->leafs.emplace_back(0, 0, grid.GetWidth(), grid.GetHeight(), 0, 0);
        }

        return;
    }

    // Adjacent level differences

    std::queue<uint64_t> codes;
    ankerl::unordered_dense::map<uint64_t, QuadrantIdentifier> mapIdentifiers;
    
    codes.emplace(0);

    mapIdentifiers.emplace(0, QuadrantIdentifier(0, 0, 2, 2, 2, 2));
    
    int adjacentShift;

    while (codes.size() > 0) {
        uint64_t parentLocationCode = codes.front();
        const QuadrantIdentifier parentQuadrant = mapIdentifiers.find(parentLocationCode)->second;

        uint64_t leafLocationCode = parentLocationCode;// << (2 * (this->resolution - parentQuadrant.level));
        auto leafLevelIterator = mapLevels.find(leafLocationCode);

        if (leafLevelIterator == mapLevels.end() 
            || leafLevelIterator->second != parentQuadrant.level) {
            
            adjacentShift = 2 * (this->resolution - parentQuadrant.level);

            for (uint64_t i = 0; i < 4; ++i) {
                this->IncrementAdjacentQuad(mapIdentifiers, parentQuadrant, i, adjacentShift);
            }
            
            adjacentShift -= 2;
            
            for (uint64_t k = 0; k < 4; ++k) {
                uint64_t childLocationCode = leafLocationCode | (k << (2 * (this->resolution - parentQuadrant.level - 1)));
            
                if (parentQuadrant.level + 1 < this->resolution) {
                    codes.emplace(childLocationCode);
                }

                auto childQuadrant = QuadrantIdentifier(childLocationCode, parentQuadrant.level + 1);
                childQuadrant.dir[k >> 1] = this->GetChildLevelDiff(parentQuadrant, k >> 1);
                childQuadrant.dir[k | 2] = this->GetChildLevelDiff(parentQuadrant, k | 2);
                childQuadrant.dir[(k ^ 3) >> 1] = 0;
                childQuadrant.dir[(k ^ 3) | 2] = 0;

                mapIdentifiers.insert_or_assign(childLocationCode, childQuadrant);
             
                this->IncrementAdjacentQuad(mapIdentifiers, childQuadrant, k >> 1, adjacentShift);
                this->IncrementAdjacentQuad(mapIdentifiers, childQuadrant, k | 2, adjacentShift);      
            }
        } else {
            //mapIdentifiers.emplace(leafLocationCode, parentQuadrant);
        }

        codes.pop();
    } 
    
    // Build graphs
    this->graph.resize(this->leafs.size());

    for (int i = 0; i < this->leafs.size(); ++i) {
        int level = this->leafs[i].GetLevel();
        uint64_t code = this->leafs[i].GetCode();
        const QuadrantIdentifier& quad = mapIdentifiers.at(code);
        for (int k = 0; k < 4; ++k) {
            int levelDiffs = quad.dir[k];

            if (levelDiffs > 0) continue;

            if (levelDiffs == 0) {
                uint64_t adjacentCode = this->GetAdjacentQuadrant(code, k, 2 * (this->resolution - level));
                
                auto adjacentIterator = this->leafIndex.find(adjacentCode);
                if (adjacentIterator == this->leafIndex.end()) continue;

                int adjacentIndex = adjacentIterator->second;
                this->graph[i].push_back(adjacentIndex);
            } else {
                int shift = 2 * (this->resolution - level - levelDiffs);
                uint64_t adjacentCode = this->GetAdjacentQuadrant((code >> shift) << shift, k, shift);
                
                auto adjacentIterator = this->leafIndex.find(adjacentCode);
                if (adjacentIterator == this->leafIndex.end()) continue;
                
                int adjacentIndex = adjacentIterator->second;
                this->graph[i].push_back(adjacentIndex);
                this->graph[adjacentIndex].push_back(i);
            }
        }
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


