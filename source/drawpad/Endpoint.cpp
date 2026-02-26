#include "Endpoint.hpp"

Endpoint::Endpoint(int x, int y, Color color) {
    this->color = color;
    this->x = x;
    this->y = y;
    this->rec = {
        .x = (float)x,
        .y = (float)y,
        .width = Endpoint::SIZE,
        .height = Endpoint::SIZE,
    };
}

void Endpoint::SetPosition(int x, int y) {
    this->rec.x = (float)x;
    this->rec.y = (float)y;
    this->x = x;
    this->y = y;
}

int Endpoint::GetX() const {
    return x;
}

int Endpoint::GetY() const {
    return y;
}

void Endpoint::Update(float deltaTime) {
    rotation += ROTATE_AMOUNT * deltaTime;
    if (rotation > 360) {
        rotation -= 360;
    }
}

void Endpoint::Render() {
    DrawRectanglePro(rec, Vector2{Endpoint::SIZE / 2, Endpoint::SIZE / 2}, rotation, color);
}