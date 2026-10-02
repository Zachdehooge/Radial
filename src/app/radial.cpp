//
// Created by zach on 9/30/26.
//
#include "core/gui/headers/window.h"

int main()
{
    window w(800, 800, "Radial");
    while (!w.shouldClose())
    {
        w.draw();
    }
}
