#include "window.h"
#include "renderer.h"
#include "string"

namespace Window
{
    GLFWwindow* window        = nullptr;
    std::string window_title  = "";
    float window_width  = 400, window_height = 300;

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

    void fullscreen()
    {
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

        glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE); // bỏ border
        glfwSetWindowMonitor(
            window,
            NULL,
            0,
            0,
            mode->width,
            mode->height,
            mode->refreshRate
        );
        window_width = mode->width;
        window_height = mode->height;
    }

    GLFWwindow* create(){
        if (!glfwInit()) {
            std::cerr << "Error: GLFW init failed!\n";
            return nullptr;
        }
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); 
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(int(window_width), int(window_height), window_title.c_str(), NULL, NULL);
        glfwHideWindow(window);
        
        if (!window) {
            std::cerr << "Failed to create window\n";
            glfwTerminate();
        }
        

        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD!\n";
        }
        
        glfwSetWindowSizeCallback(window, resize);

        Render::init(window);
        //glfwSwapInterval(0);
        return window;
    }

    void centerWindow()
    {
        GLFWmonitor* primary = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(primary);

        int win_w, win_h;
        glfwGetWindowSize(window, &win_w, &win_h);

        int pos_x = (mode->width - win_w) / 2;
        int pos_y = (mode->height - win_h) / 2;

        glfwSetWindowPos(window, pos_x, pos_y);
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
}
