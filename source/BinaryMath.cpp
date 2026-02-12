#include <cstdint>

namespace BinaryMath {
    /**
        Interleaving Algorithm from Daniel Lemire's blog
        How fast can you bit-interleave 32-bit integers?
        and
        https://stackoverflow.com/questions/4909263/how-to-efficiently-de-interleave-bits-inverse-morton
    */

    uint64_t InterleaveZero(uint32_t input) {
        uint64_t word = input;
        word = (word ^ (word << 16)) & 0x0000ffff0000ffff;
        word = (word ^ (word << 8 )) & 0x00ff00ff00ff00ff;
        word = (word ^ (word << 4 )) & 0x0f0f0f0f0f0f0f0f;
        word = (word ^ (word << 2 )) & 0x3333333333333333;
        word = (word ^ (word << 1 )) & 0x5555555555555555;
        return word;
    }


    uint64_t Interleave(uint32_t x, uint32_t y)  {
        return InterleaveZero(x) | (InterleaveZero(y) << 1);
    }


    void Deinterleave(uint64_t z, uint64_t& x, uint64_t& y)  {
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

    
    uint32_t Deinterleave(uint64_t z)  {
        uint64_t x;
        x = z & 0x5555555555555555;
        x = (x | (x >> 1)) & 0x3333333333333333;
        x = (x | (x >> 2)) & 0x0f0f0f0f0f0f0f0f;
        x = (x | (x >> 4)) & 0x00ff00ff00ff00ff;
        x = (x | (x >> 8)) & 0x0000ffff0000ffff;
        return x;
    }
}