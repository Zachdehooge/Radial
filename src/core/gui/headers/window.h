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
    bool FirstExampleButton = false;
    bool SecondExampleButton = false;
// TODO: Need to add buttons to skip, replay, load, and start/stop loaded LVL II data 
};
