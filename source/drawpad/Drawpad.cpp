#include "raylib.h"

#include "Program.hpp"
#include "Drawpad.hpp"

Drawpad::Drawpad() {
    radius = 10;
    currentMousePosition = Vector2();
    currentMode = DrawpadMode::DO_NOTHING;
    toggleErase = false;
}


void Drawpad::Init() {
    drawpadImage = GenImageColor(WINDOW_W, WINDOW_H, WHITE);
    ImageFormat(&drawpadImage, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8); // Convert to 32-bit RGBA
   
    drawpadTexture = LoadRenderTexture(WINDOW_W, WINDOW_H);
    UpdateTexture(drawpadTexture.texture, drawpadImage.data);
}

const Color* Drawpad::GetPixels() {
    return (Color*) drawpadImage.data;
}

void Drawpad::DrawCircle(Color color) {
    for (int y = -radius; y < radius; ++y) {
        for (int x = -radius; x < radius; ++x) {
            if (x * x + y * y <= radius * radius) {
                ImageDrawPixel(&drawpadImage, currentMousePosition.x + x, currentMousePosition.y + y, color);
            }
        }
    }

    UpdateTexture(drawpadTexture.texture, drawpadImage.data); // Initial upload
}


void Drawpad::Input() {
    currentMousePosition = GetMousePosition();
    
    if (IsKeyPressed(KEY_E)) {
        toggleErase = !toggleErase;
    }

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        if (!toggleErase) {
            currentMode = DrawpadMode::DRAW;
        } else {
            currentMode = DrawpadMode::ERASE;
        }
    } else {
        currentMode = DrawpadMode::DO_NOTHING;
    }

    if (IsKeyDown(KEY_A)) {
        radius += 1;
        radius = radius > 200 ? 200 : radius;
    }

    if (IsKeyDown(KEY_S)) {
        radius -= 1;
        radius = radius < 10 ? 10 : radius;
    }

}


void Drawpad::Update() {
    if (currentMode == DrawpadMode::DO_NOTHING) {
        return;
    }

    if (currentMode == DrawpadMode::DRAW) {
        this->DrawCircle(BLACK);
        return;
    }

    if (currentMode == DrawpadMode::ERASE) {
        this->DrawCircle(WHITE);
        return;
    }
}


void Drawpad::Render() {
    DrawTextureRec(
        drawpadTexture.texture, 
    (Rectangle) {
        .x = 0,
        .y = 0,
        .width = WINDOW_W,
        .height = WINDOW_H,
    }, (Vector2) {0, 0}, WHITE);

    Color brushColor = toggleErase ? DARKBROWN : DARKGREEN;
    DrawRing(currentMousePosition, radius - 5, radius, 0, 360, 1, brushColor);
}


Drawpad::~Drawpad() {
    UnloadImage(drawpadImage);
    UnloadRenderTexture(drawpadTexture);
}