#pragma once

#include <cstddef>
#include <vector>


class GridEnvironment {
public:
    virtual const bool IsValid(int i) const = 0;

    const size_t GetWidth() const {
        return gridWidth;
    }
    
    const size_t GetHeight() const {
        return gridHeight;
    }

protected:
    size_t gridWidth;
    size_t gridHeight;
};

