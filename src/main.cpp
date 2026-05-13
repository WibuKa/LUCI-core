#include <cstdio>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include "window.h"
#include "audio.h"
#include "core/input_system.h"
#include "game.h"
#include "version.h"

namespace Time {
    double deltaTime = 0.0;
    double lastFrame = 0.0;
    const double targetFrameTime = 1.0 / 60.0; // 60 FPS
}

int main() {
    std::cout << "-- " << GameEngineName << " " << luciVersion
              << " | " << architecture << "\n";
    Window::create();
    Audio::init();
    Game::load();
    Window::show();


    while (!Window::isClose()) {
        double currentTime = glfwGetTime();
        Time::deltaTime = currentTime - Time::lastFrame;

        if (Time::deltaTime < Time::targetFrameTime) {
            while ((glfwGetTime() - Time::lastFrame) < Time::targetFrameTime) {
            }
            currentTime = glfwGetTime();
            Time::deltaTime = currentTime - Time::lastFrame;
        }

        Time::lastFrame = currentTime;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Window::update();
        Input::update();
        Game::update(Time::deltaTime);

        glfwSwapBuffers(Window::window);
    }
    // Clean Up
    Audio::close();
    Window::close();
    return 0;
}
