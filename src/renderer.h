#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <tuple>
#include <color.h>
#include "texture_region.h"
#include "font.h"

#define UNIFORM_INT 1
#define UNIFORM_FLOAT 2
#define UNIFORM_VEC2 3
#define UNIFORM_VEC3 4
#define UNIFORM_VEC4 5

struct GLFWwindow;

struct RenderTexture
{
    std::string path;
    GLuint textureID;
    int width,height;
    int grid_x,grid_y;
};

struct Vertex {
    float x, y;
    float u, v;
    float r, g, b, a;
};


struct Quad {
    float x0, y0;
    float x1, y1;
    float x2, y2;
    float x3, y3;
};

extern std::unordered_map<std::string,unsigned int> pathCache;

namespace Render {

    extern int windowWidth, windowHeight;

    void init();
    void resize(int width, int height);
    void setTime(float time);
    void setZoom(float value);
    void setFont(Font font);

    unsigned int get_texture_id(const std::string &path);
    bool has_texture(std::string path);
    void get_textureSize(unsigned int ID, int& width, int& height);
    std::tuple<int,int> getWindowSize();
    std::tuple<int,int> getTextureSize(unsigned id);

    Texture createTexture(uint8_t* data ,int width, int height, int channel);

    void createVAOnVBO(GLuint& vao, GLuint& vbo);
    void createTilemapMesh(int ID, const std::vector<int>& tiles, int W, int H, int Row, int Col, int pixelSize);

    void beginBatch();
    void endBatch();
    void submitSprite(
        GLuint textureID, 
        float x, float y, float tw, float th,
        float ox, float oy, float ow, float oh, 
        float scale_x, float scale_y, float angle
    );
    void flush();
    //--------------------------------------------------- draw -------------------------------------------------------//
    void drawText(const std::string& text,float x,float y,const std::string& align);
    void drawCircle(float x, float y, float r, bool fill);
    void drawSprite(TextureRegion& texture_region, float x, float y, float angle, float scale_x, float scale_y);
    void drawRectangle(float x, float y, float w, float h, bool fill);
    
    //-------------------------------------------------- shader ------------------------------------------------------//
    unsigned int loadShader(std::string shader_Path);
    void useShader(unsigned int ID = 0);
    void setShader(unsigned int id);

    //--------------------------------------------------- Uniform -----------------------------------------------------//
    void setColor(float r, float g, float b, float a);
    void setTime(float rt);
    void setViewTranslate(float x, float y);
    void setUniformBool(GLuint shader, std::string name, bool v);
    void setUniformInt(GLuint shader, const std::string &name, int value);
    void setUniformFloat(GLuint shader, const std::string &name, float value);
    void setUniformVec2(GLuint shader, const std::string &name, float x, float y);
    void setUniformVec3(GLuint shader, const std::string &name, float x, float y, float z);
    void setUniformVec4(GLuint shader, const std::string &name, float x, float y, float z, float w);
}
