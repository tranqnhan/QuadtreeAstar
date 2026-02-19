#pragma once

#include "raylib.h"

enum DrawpadMode {
    DO_NOTHING,
    DRAW,
    ERASE
};


class Drawpad {
public:
    Drawpad();

    void Init();
    void Input();
    void Update();
    void Render();

    const Color* GetPixels();

    ~Drawpad();
    
private:
    int radius;
    bool toggleErase;

    Vector2 currentMousePosition;
    DrawpadMode currentMode;
    Image drawpadImage;
    RenderTexture drawpadTexture;

    void DrawCircle(Color color);
};