#pragma once

#include "raylib.h"

#include "GridEnvironment.hpp"

class ImageGridEnvironment : public GridEnvironment {
public:
    void Init(const Color *pixels, size_t width, size_t height) {
        this->pixels = pixels;
        gridWidth = width;
        gridHeight = height;
    }
        
    const bool IsValid(int i) const;

private:
    const Color *pixels;

};