#include "window.h"
#include "renderer.h"
#include "string"
#include "delog.h"

#define FULLSCREEN_MODE           0
#define WINDOWED_MODE             1
#define BORDERLESS_WINDOWED_MODE  2

namespace Window
{
    GLFWwindow* window        = nullptr;
    std::string window_title  = "";

    int window_width  = 400, window_height = 300;
    int window_position_x = 0, window_position_y = 0;

    int window_mode = WINDOWED_MODE;

    void resize(GLFWwindow* window, int width, int height){
        Render::resize(width,height);
    }

    void set_resizable(bool opt)
    {
        glfwSetWindowAttrib(window, GLFW_RESIZABLE, opt);
    }

    void set_window(std::string title,int width,int height)
    {
        window_title  = title;
        window_width  = width;
        window_height = height;
    }

    void set_size(int w, int h)
    {
        window_width  = w;
        window_height = h;
    }

    void set_tile(std::string tile)
    {
        window_title  = tile;
    }

    void set_fullscreen()
    {
        window_mode = FULLSCREEN_MODE;

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(
            window,
            monitor,
            0,
            0,
            mode->width,
            mode->height,
            mode->refreshRate
        );
    }

    void set_windowed(int width, int height)
    {
        window_mode = WINDOWED_MODE;

        if(width  < 0) width  = 0;
        if(height < 0) height = 0;

        window_width = width;
        window_height = height;

        glfwSetWindowMonitor(
            window,
            nullptr,
            window_position_x, 
            window_position_y, 
            window_width, 
            window_height, 
            GLFW_DONT_CARE
        );
    }

    void set_borderless_windowed()
    {
        window_mode = BORDERLESS_WINDOWED_MODE; 

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);

        glfwSetWindowMonitor(
            window,
            nullptr,
            0,
            0,
            mode->width,
            mode->height,
            mode->refreshRate
        );
    }

    GLFWwindow* create(){
        if (!glfwInit()) {
            Delog::error("Error: GLFW init failed!");
            return nullptr;
        }
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); 
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(int(window_width), int(window_height), window_title.c_str(), NULL, NULL);
        glfwHideWindow(window);
        
        if (!window) {
            Delog::error("Failed to create window");
            glfwTerminate();
        }
        

        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            Delog::error("Failed to initialize GLAD!");
        }
        
        glfwSetWindowSizeCallback(window, resize);
        glfwSwapInterval(0);
        return window;
    }

    void centerWindow()
    {
        GLFWmonitor* primary = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(primary);

        int win_w, win_h;
        glfwGetWindowSize(window, &win_w, &win_h);

        window_position_x = (mode->width - win_w) / 2;
        window_position_y = (mode->height - win_h) / 2;

        glfwSetWindowPos(window, window_position_x, window_position_y);
    }

    void show(){
        glfwSetWindowTitle(window,window_title.c_str());
        glfwSetWindowSize(window, window_width, window_height);
        centerWindow();
        glfwShowWindow(window);
    }

    void update(){
        glfwPollEvents();
    }

    bool isClose()
    {
        return glfwWindowShouldClose(window);
    }

    void close(){
        glfwTerminate();
    }

    int get_width()
    {
        return window_width;
    }

    int get_height()
    {
        return window_height;
    }
}
