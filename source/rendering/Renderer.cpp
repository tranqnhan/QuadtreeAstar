#include <raylib.h>

#include "Renderer.hpp"


void Renderer::DrawQuadrant(const Quadrant& quad) {
    DrawRectangleLines(quad.GetX() + 1, quad.GetY(), quad.GetWidth() - 1, quad.GetHeight() - 1, PURPLE);
}
