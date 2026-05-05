#include "input_system.h"
#include "key.h"
#include "window.h"
#include "cstring"

namespace Input
{
    bool keyState[GLFW_KEY_LAST + 1] = {};
    bool lastKeyState[GLFW_KEY_LAST + 1] = {};
    bool mouseState[GLFW_MOUSE_BUTTON_LAST + 1] = {};
    bool lastMouseState[GLFW_MOUSE_BUTTON_LAST + 1] = {};

    void update()
    {
        memcpy(lastKeyState, keyState, sizeof(keyState));
        memcpy(lastMouseState, mouseState, sizeof(mouseState));

        for (int key = 0; key <= GLFW_KEY_LAST; ++key)
            keyState[key] =
                (glfwGetKey(Window::window, key) == GLFW_PRESS);

        for (int btn = 0; btn <= GLFW_MOUSE_BUTTON_LAST; ++btn)
            mouseState[btn] =
                (glfwGetMouseButton(Window::window, btn) == GLFW_PRESS);
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
