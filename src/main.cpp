#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include "delog.h"
#include "system.h"
#include "window.h"
#include "audio.h"
#include "input_system.h"
#include "game.h"
#include "version.h"
#include "timer.h"
#include "renderer.h"
#include "loader.h"

int main() {
    Delog::msg("-- %s %s | %s", GameEngineName, luciVersion, architecture);
    System::init();
    Window::create();
    Render::init();
    Audio::init();
    Loader::init();
    Game::init();
    Window::show();

    while (!Window::isClose()) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Window::update();
        Input::update();
        Game::update(Time::deltaTime);
        Game::draw();
        glfwSwapBuffers(Window::window);

        double currentTime = glfwGetTime();
        Time::deltaTime = currentTime - Time::lastFrame;
        if (Time::deltaTime < Time::targetFrameTime) {
            while ((glfwGetTime() - Time::lastFrame) < Time::targetFrameTime) {
            }
            currentTime = glfwGetTime();
            Time::deltaTime = currentTime - Time::lastFrame;
        }
        Time::lastFrame = currentTime;

    }
    // Clean Up
    Audio::close();
    Window::close();
    return 0;
}
