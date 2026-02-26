#pragma once

#include "raylib.h"

class Endpoint {
public:
    Endpoint(int x, int y, Color color);
    void SetPosition(int x, int y);
    void Update(float deltaTime);
    void Render();

    int GetX() const;
    int GetY() const;

private:
    float rotation;
    Color color;
    Rectangle rec;

    int x, y;

    static constexpr float ROTATE_AMOUNT = 180.0;
    static constexpr float SIZE = 20.0;

};