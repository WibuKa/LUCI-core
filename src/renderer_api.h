#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <color.h>

struct GLFWwindow;

extern std::unordered_map<std::string,unsigned int> pathCache;

namespace Render{
    unsigned int load_texture(const std::string &path,int cx,int cy);
    unsigned int get_texture_id(const std::string &path);

    std::tuple<int,int> getWindowSize();
    std::tuple<int,int> getTextureSize(unsigned id);

    void setZoom(float value);
    void set_Time(const float time);
    void set_font(int id);
    int load_font(std::string path, int font_size);
    void init(GLFWwindow* window);
    void resize(const int width,const int height);
    void importShader(const char* FragPath, const char* vertPath);
    //----------------------  Draw   ----------------------//
    void drawText(const std::string& text, float x, float y, const std::string& align);
    void drawSprite(unsigned int id,float x,float y,float scale_x,float scale_y,float angle, int frame);
    void drawSquare(float x,float y,float w,float h,bool fill);
    void drawCircle(float x,float y,float r,bool fill);
    //----------------------  Shader ----------------------//
    void loadShader(std::string shader_Path);
    void pushShader(std::string shader_Path);
    void pushShaderID(unsigned int ShaderID);
    void popShader();
    //---------------------- Uniform ----------------------//
    void setColor(float r,float g,float b,float a);
    void setViewTranslate(float x, float y);
    void setUniformBool(std::string name,bool v);
    void setUniformInt(std::string name,int v);
    void setUniformFloat(std::string name,float v);
    void setUniformVec2(std::string name,float v1, float v2);
    void setUniformVec3(std::string name,float v1, float v2, float v3);
    void setUniformVec4(std::string name,float v1, float v2, float v3, float v4);
}