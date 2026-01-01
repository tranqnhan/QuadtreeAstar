#include <raylib.h>

#include "Renderer.hpp"


void Renderer::DrawQuadrant(const QuadtreeLeaf& quad) {
    DrawRectangleLines(quad.getX(), quad.getY(), quad.getWidth(), quad.getHeight(), PURPLE);
}
