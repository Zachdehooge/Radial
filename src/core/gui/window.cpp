//
// Created by zach on 10/1/26.
//

#include "headers/window.h"
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "headers/style_genesis.h"

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
    GuiLoadStyleCherry();
    GuiLoadStyleGenesis();
    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    if (GuiButton(Rectangle{ 24, 24, 120, 30 }, "#191#Show Message")) FirstExampleButton = true;

    if (GuiButton(Rectangle{70, 70, 120, 30}, "Some Example Text")) SecondExampleButton = true;

    if (FirstExampleButton)
    {
        int btnActive = -1;
        GuiMessageBox(Rectangle{ 85, 70, 250, 100 },
            "#191#Message Box", "Hi! This is a message!", "Nice;Cool", &btnActive);

        if (btnActive >= 0) FirstExampleButton = false;
    }
    if (SecondExampleButton)
    {
        int btnActive = -1;
        GuiMessageBox(Rectangle{ 85, 70, 250, 100 },
            "#191#Example Text", "Main Title Message!", "First;Second;Third", &btnActive);

        if (btnActive >= 0) SecondExampleButton = false;
    }

    EndDrawing();
}
