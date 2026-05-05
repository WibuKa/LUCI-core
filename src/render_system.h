#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <array>
#include <color.h>


struct Texture
{
    std::string path;
    GLuint textureID;
    int width,height;
    int grid_x,grid_y;
};


struct Uniform
{
    std::string name;
    char type;
    float value[4];
};

extern GLuint VAO_default;

namespace RenderSYS {

    extern int windowWidth, windowHeight;

    void init(GLFWwindow* glwd);
    void resize(int width, int height);
    void setRunTime(float rt);
    
    bool has_texture(std::string path);
    void get_textureSize(unsigned int ID,int& width, int& height);

    void createVAOnVBO(GLuint& vao, GLuint& vbo);
    void createTilemapMesh(int ID,const std::vector<int>& tiles, int W, int H,int Row, int Col, int pixelSize);

    unsigned int createTexture(const std::string &path,int grid_x, int grid_y);
    unsigned int createTexture_data(unsigned char* data,int width,int height,int channel,int grid_x, int grid_y);
    //--------------------------------------------------- draw  -------------------------------------------------------//
    void add_sprite(unsigned int id,float x,float y,float scale_x = 1.0,float scale_y = 1.0,float angle = 0.0f,int frame = 0);
    void add_quad(float x = 0,float y = 0,float w = 0,float h = 0,float angle = 0);
    void draw(GLuint textureID,float x, float y, float w, float h,float scale_x,float scale_y,float angle,glm::vec2 frameOffset,glm::vec2 frameSize,GLuint vertexCount = 6,GLuint VAO = VAO_default);
    //--------------------------------------------------  shader ------------------------------------------------------//
    void reload_resolution();
    void useShader(unsigned int ID = 0);
    void loadFragmentShaderSource(std::string path);
    void createShader(unsigned int ver, unsigned int frag,unsigned int& ShaderID);
    //--------------------------------------------------- Uniform -----------------------------------------------------//
    void setColor(float r,float g,float b,float a);
    void setViewTranslate(float x, float y);
    void setUniformInt(const std::string &name, int value);
    void setUniformFloat(const std::string &name, float angle);
    void setUniformVec2(const std::string &name, float x, float y);
    void setUniformVec3(const std::string &name, float x, float y, float z);
    void setUniformVec4(const std::string &name, float x, float y, float z, float w);
};