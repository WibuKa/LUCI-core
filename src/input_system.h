#pragma once
#include <string>
#include <tuple>

namespace Input
{
    void update();

    int  getKey(const std::string& name);

    bool isKeyDown(const int& key);
    bool isKeyPress(const int& key);
    bool isKeyReleased(const int& key);

    bool isKeyboardDown(const std::string& name);
    bool isKeyboardPress(const std::string& name);
    bool isKeyboardReleased(const std::string& name);

    bool isMouseDown(int key);
    bool isMousePress(int key);
    bool isMouseReleased(int key);

    std::tuple<float, float> getMousePos();
}