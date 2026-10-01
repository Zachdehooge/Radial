//
// Created by zach on 10/1/26.
//
#pragma once

class window
{
public:
    window(int width, int height, const char* title);
    ~window();

    bool shouldClose() const;
    void draw();

private:
    bool showMessageBox = false;
};
