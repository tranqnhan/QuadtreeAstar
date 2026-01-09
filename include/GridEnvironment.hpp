#pragma once

#include <cstddef>
#include <vector>

class GridEnvironment {

public:

    GridEnvironment(size_t gridWidth, size_t gridHeight) 
        : gridHeight(gridHeight), gridWidth(gridWidth) 
    {
        grid.resize(gridHeight * gridWidth);
    }

    void SetValid(int i, bool isValid) {
        if (i < grid.size()) {
            grid[i] = isValid;
        }
    }

    const bool IsValid(int i) const {
        if (i < grid.size()) {
            return grid[i];
        }
        return false;
    }

    const size_t GetWidth() const {
        return gridWidth;
    }
    
    const size_t GetHeight() const {
        return gridHeight;
    }

private:
    std::vector<bool> grid;
    size_t gridWidth;
    size_t gridHeight;
};