#include "ImageGridEnvironment.hpp"

const bool ImageGridEnvironment::IsValid(int i) const {
    return pixels[i].r != 0; 
}