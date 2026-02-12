#pragma once

#include <cstdint>

namespace BinaryMath {
    uint64_t Interleave(uint32_t x, uint32_t y);
    uint64_t InterleaveZero(uint32_t input);
    void Deinterleave(uint64_t z, uint64_t& x, uint64_t& y);
    uint32_t Deinterleave(uint64_t z);
};