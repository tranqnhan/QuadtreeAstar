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


Quadtree::Quadtree() {

}

void Quadtree::Init(int size) {
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


void Quadtree::SubdivideRegionSmall(uint64_t fromIndex, uint64_t upperBound, bool oldValid) {
    const uint64_t mask = 0xFFFFFFFFFFFFFFE;
    int shift = __builtin_ctz(fromIndex) & mask;
    uint64_t tempIndex = fromIndex >> shift;

    uint64_t code; 
    
    while (tempIndex != 0) {
        int k = 0b11 & tempIndex;

        while (k > 0 && k < 4) {
            code = (tempIndex++) << shift;
            
            if (code >= upperBound) return;
            
            this->leafIndex.emplace(code, this->leafs.size());
            this->leafs.emplace_back(code, this->resolution - (shift >> 1), oldValid);
            
            k++;
        }

        tempIndex >>= 2;
        shift+=2;
    }
}


void Quadtree::SubdivideRegionLarge(uint64_t fromIndex, uint64_t lowerBound, bool oldValid) {
    const uint64_t mask = 0xFFFFFFFFFFFFFFE;
    int shift = __builtin_ctz(fromIndex) & mask;
    uint64_t tempIndex = fromIndex >> shift;

    uint64_t code; 
    
    while (tempIndex != 0) {
        int k = 0b11 & tempIndex;

        while (k > 0) {
            code = (--tempIndex) << shift;
            
            if (code < lowerBound) {
                code = (++tempIndex) << shift;
                this->SubdivideRegionSmall(lowerBound, code, oldValid);
                return;
            }

            this->leafIndex.emplace(code, this->leafs.size());
            this->leafs.emplace_back(code, this->resolution - (shift >> 1), oldValid);
            
            if (code == lowerBound) return;
            
            k--;
        }

        tempIndex >>= 2;
        shift += 2;
    }
}


void Quadtree::BuildRegion(const GridEnvironment& grid) {
     uint64_t x, y;
    
     bool oldValid = grid.IsValid(0);
     uint64_t oldIndex = 0;

    const uint64_t size = grid.GetWidth() * grid.GetHeight();

    for (uint64_t newIndex = 1; newIndex < size; ++newIndex) {
        BinaryMath::Deinterleave(newIndex, x, y);
        bool newValid = grid.IsValid(y * grid.GetWidth() + x);

        if (newValid == oldValid) continue;

        this->SubdivideRegionLarge(newIndex, oldIndex, oldValid);

        oldIndex = newIndex;
        oldValid = newValid;
    }

    this->SubdivideRegionSmall(oldIndex, size, oldValid);
}



/**
 * Implementation of Linear Quadtree with Level Differences from
 * A Constant-Time Algorithm for Finding Neighbors in Quadtrees
 * by Kunio Aizawa and Shojiro Tanaka 
 */

void Quadtree::BuildLevelDifferences(ankerl::unordered_dense::map<uint64_t, QuadrantIdentifier> &mapIdentifiers) {

    // Adjacent level differences
    std::queue<uint64_t> codes;
    
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
                uint64_t childLocationCode = leafLocationCode | (k << adjacentShift);
            
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
}

void Quadtree::BuildGraph(const ankerl::unordered_dense::map<uint64_t, QuadrantIdentifier> &mapIdentifiers) {
  // Build graphs
    this->graph.resize(this->leafs.size());

    for (int i = 0; i < this->leafs.size(); ++i) {
        if (!this->leafs[i].IsValid()) continue;

        const int level = this->leafs[i].GetLevel();
        const uint64_t code = this->leafs[i].GetCode();
        const QuadrantIdentifier& quad = mapIdentifiers.at(code);
        
        for (int k = 0; k < 4; ++k) {
            const int levelDiffs = quad.dir[k];

            if (levelDiffs > 0) continue;

            if (levelDiffs == 0) {
                uint64_t adjacentCode = this->GetAdjacentQuadrant(code, k, 2 * (this->resolution - level));
                
                auto adjacentIterator = this->leafIndex.find(adjacentCode);
                if (adjacentIterator == this->leafIndex.end()) continue;

                int adjacentIndex = adjacentIterator->second;
                
                if (!this->leafs[adjacentIndex].IsValid()) continue;

                this->graph[i].push_back(adjacentIndex);
            } else {
                const int shift = 2 * (this->resolution - level - levelDiffs);
                uint64_t adjacentCode = this->GetAdjacentQuadrant((code >> shift) << shift, k, shift);
                
                auto adjacentIterator = this->leafIndex.find(adjacentCode);
                if (adjacentIterator == this->leafIndex.end()) continue;
                
                int adjacentIndex = adjacentIterator->second;

                if (!this->leafs[adjacentIndex].IsValid()) continue;

                this->graph[i].push_back(adjacentIndex);
                this->graph[adjacentIndex].push_back(i);
            }
        }
    }
}

void Quadtree::Build(const GridEnvironment& grid) {
    this->graph.clear();
    this->leafs.clear();
    this->leafIndex.clear();

    // ---------- //
    this->BuildRegion(grid);
    // ---------- //

    if (this->leafs.size() > 0) {
        // ---------- //
        ankerl::unordered_dense::map<uint64_t, QuadrantIdentifier> mapIdentifiers;
        this->BuildLevelDifferences(mapIdentifiers);
        // ---------- //

        // ---------- //
        this->BuildGraph(mapIdentifiers);
        // ---------- //
    }

    //std::printf("num leafs %li\n", this->leafs.size());
}
