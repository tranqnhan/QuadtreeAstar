#include <algorithm>
#include <cmath>
#include <cstdint>

#include <cstdio>
#include <queue>
#include <vector>

#include <ankerl/unordered_dense.h>
#include <raylib.h>

#include "BinaryMath.hpp"
#include "Quadtree.hpp"
#include "GridEnvironment.hpp"

// Quadtree Leaf
Quadrant::Quadrant(uint64_t locationCode, int level) {
    this->locationCode = locationCode;
    this->level = level;
}

// Quadtree
Quadtree::Quadtree(int size) {
    this->resolution = (int)std::log2(size);
    this->resolution = resolution > 32 ? 32 : resolution;
    const int push = (32 - resolution) * 2;

    // SOUTH
    this->dI[0] = BinaryMath::Interleave(0, (uint32_t)(~0) >> (push / 2));
    // NORTH
    this->dI[1] = 0b10;
    // WEST
    this->dI[2] = BinaryMath::Interleave((uint32_t)(~0) >> (push / 2), 0);
    // EAST
    this->dI[3] = 0b01;

    // tx = 01...0101 “01” repeated r times
    // ty = 10...1010 “10” repeated r times
    this->tx = 0x5555555555555555 >> push;
    this->ty = 0xAAAAAAAAAAAAAAAA >> push;
    std::printf("push: %i\nres: %i\ntx %lb \nty %lb\n", push, resolution, this->tx, this->ty);
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


void Quadtree::Build(const GridEnvironment& grid) {
    const char numQuads = 4;
    const size_t size = this->resolution * 3 + 1;

    // TODO: replace with a calculated the bounds of this
    std::vector<Region> stack(size);
    std::vector<int> levels(size);
    std::vector<uint64_t> zcodes(size);
    uint64_t head = 0;
    uint64_t x, y;
    Region region;

    int maxHead = 0;

    for (uint64_t z = 0; z < grid.GetWidth() * grid.GetHeight(); z += numQuads) {
        for (int i = 0; i < numQuads; ++i) {
            BinaryMath::Deinterleave(z + i, x, y);
            levels[head] = this->resolution;
            zcodes[head] = z;
            stack[head++] = grid.IsValid(y * grid.GetWidth() + x) ? Region::Valid : Region::Block;
            maxHead = head > maxHead ? head : maxHead;
        }

        do {
            head -= numQuads;
            region = stack[head + 0];
            
            for (int j = 1; j < numQuads; ++j) {
                if (stack[head + j] != region) {
                    region = Region::Mixed;
                    // TODO: Room for optimization here
                    // Quadtree Leafs
                    for (int k = 0; k < numQuads; ++k) {
                        if (stack[head + k] == Region::Mixed) continue;

                        int level = levels[head + k];

                        uint64_t code = zcodes[head + k];

                        this->leafIndex.emplace(code, this->leafs.size());
                        this->leafs.emplace_back(code, level);
                        this->leafValid.emplace_back(stack[head + k] == Region::Valid);
                    }

                    break; 
                }
            }

            levels[head] -= 1;
            stack[head++] = region;

            maxHead = head > maxHead ? head : maxHead;

        } while (head >= numQuads && levels[head - 1] == levels[head - 4]);
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
        auto leafLevelIterator = this->leafIndex.find(leafLocationCode);

        if (leafLevelIterator == this->leafIndex.end() 
            || this->leafs[leafLevelIterator->second].GetLevel() != parentQuadrant.level) {
            
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
        }

        codes.pop();
    } 
    
    // Build graphs
    this->graph.resize(this->leafs.size());

    for (int i = 0; i < this->leafs.size(); ++i) {
        if (!this->leafValid[i]) continue;

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
                
                if (!this->leafValid[adjacentIndex]) continue;

                this->graph[i].push_back(adjacentIndex);
            } else {
                int shift = 2 * (this->resolution - level - levelDiffs);
                uint64_t adjacentCode = this->GetAdjacentQuadrant((code >> shift) << shift, k, shift);
                
                auto adjacentIterator = this->leafIndex.find(adjacentCode);
                if (adjacentIterator == this->leafIndex.end()) continue;
                
                int adjacentIndex = adjacentIterator->second;

                if (!this->leafValid[adjacentIndex]) continue;

                this->graph[i].push_back(adjacentIndex);
                this->graph[adjacentIndex].push_back(i);
            }
        }
    }

}
