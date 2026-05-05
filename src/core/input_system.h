#pragma once
#include <string>
#include <tuple>

namespace Input
{
    void update();

    int  getKey(const std::string& name);

    bool isKeyDown(const std::string& name);
    bool isKeyPress(const std::string& name);
    bool isKeyReleased(const std::string& name);

    bool isMouseDown(int key);
    bool isMousePress(int key);
    bool isMouseReleased(int key);

    std::tuple<float, float> getMousePos();
}