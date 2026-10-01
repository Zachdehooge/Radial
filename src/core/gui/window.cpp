//
// Created by zach on 10/1/26.
//

#include "headers/window.h"
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

window::window(int width, int height, const char* title)
{
    InitWindow(width, height, title);
    SetTargetFPS(60);
}

window::~window()
{
    CloseWindow();
}

bool window::shouldClose() const
{
    return WindowShouldClose();
}

void window::draw()
{
    BeginDrawing();
    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    if (GuiButton(Rectangle{ 24, 24, 120, 30 }, "#191#Show Message")) showMessageBox = true;

    if (showMessageBox)
    {
        int btnActive = -1;
        GuiMessageBox(Rectangle{ 85, 70, 250, 100 },
            "#191#Message Box", "Hi! This is a message!", "Nice;Cool", &btnActive);

        if (btnActive >= 0) showMessageBox = false;
    }
    EndDrawing();
}