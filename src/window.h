#pragma once
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <string>

namespace Window
{
    extern GLFWwindow* window;
    extern bool closeWindow;

    GLFWwindow* create();
    void show();
    bool isClose();
    void update();
    void close();
    void center();

    int get_width();
    int get_height();

    //config
    void set_resizable(bool b);
    void set_window(std::string title,int width,int height);
    void set_fullscreen();
    void set_windowed(int width, int height);
    void set_borderless_windowed();
}
