#include "input_system.h"
#include "key.h"
#include "window.h"
#include <unordered_map>

namespace Input
{
    std::unordered_map<int, bool> keyState;
    std::unordered_map<int, bool> lastKeyState;

    std::unordered_map<int, bool> mouseState;
    std::unordered_map<int, bool> lastMouseState;

    void update()
    {
        lastKeyState   = keyState;
        lastMouseState = mouseState;

        for (auto& [name, key] : keyMap)
        {
            keyState[key] =
                (glfwGetKey(Window::window, key) == GLFW_PRESS);
        }

        mouseState[GLFW_MOUSE_BUTTON_LEFT] =
            (glfwGetMouseButton(Window::window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
        mouseState[GLFW_MOUSE_BUTTON_RIGHT] =
            (glfwGetMouseButton(Window::window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
        mouseState[GLFW_MOUSE_BUTTON_MIDDLE] =
            (glfwGetMouseButton(Window::window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS);
    }

    int getKey(const std::string& name)
    {
        auto it = keyMap.find(name);
        if (it != keyMap.end()) return it->second;
        return GLFW_KEY_UNKNOWN;
    }

    bool isKeyDown(const std::string& name)
    {
        int key = getKey(name);
        if (key == GLFW_KEY_UNKNOWN) return false;
        return keyState[key];
    }

    bool isKeyPress(const std::string& name)
    {
        int key = getKey(name);
        if (key == GLFW_KEY_UNKNOWN) return false;
        return keyState[key] && !lastKeyState[key];
    }

    bool isKeyReleased(const std::string& name)
    {
        int key = getKey(name);
        if (key == GLFW_KEY_UNKNOWN) return false;
        return !keyState[key] && lastKeyState[key];
    }

    bool isMouseDown(int key)
    {
        return mouseState[key];
    }

    bool isMousePress(int key)
    {
        return mouseState[key] && !lastMouseState[key];
    }

    bool isMouseReleased(int key)
    {
        return !mouseState[key] && lastMouseState[key];
    }

    std::tuple<float, float> getMousePos()
    {
        double x, y;
        glfwGetCursorPos(Window::window, &x, &y);
        return { (float)x, (float)y };
    }
}