// In Render.cpp
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <algorithm>
#include <fstream>
#include "render_system.h"
#include "shader_archive.h"

GLFWwindow* window;

float runTime;

std::unordered_map<int, std::pair<GLint, GLenum>> glFormats = {
    {1, {GL_RED,  GL_RED}},
    {2, {GL_RG,   GL_RG}},
    {3, {GL_RGB,  GL_RGB}},
    {4, {GL_RGBA, GL_RGBA}},
};

std::vector<Texture> textures;

GLuint vertex_default;

std::vector<GLuint> shaders;

unsigned int targetShader;

unsigned int defaultShader;
unsigned int geometryShader;
unsigned int fontShader;
unsigned int tilemapShader;

unsigned int fontTexture;

GLuint currentShader  = 0;
GLuint currentTexture = 0;
GLuint currentVAO     = 0;

GLuint VAO_default;

float vertices[] = {
    1.0f,  1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f, 0.0f,
    -1.0f,  1.0f, 0.0f, 1.0f
};

unsigned int indices[] = { 0, 1, 3, 1, 2, 3 };

struct Vertex {
    float x,y,u,v;
};


namespace RenderSYS
{

int windowWidth, windowHeight;

GLuint genShader(GLenum type,const char* shaderCode);

void init(GLFWwindow* glwd) {

    window = glwd;
    
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    GLuint VBO,EBO;
    
    glGenBuffers(1, &EBO);

    createVAOnVBO(VAO_default,VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //alpha color
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //shader.h
    GLuint fagID;
    vertex_default = genShader(GL_VERTEX_SHADER,ShaderArc::vertex_default_shader.c_str());
    
    // Default
    fagID = genShader(GL_FRAGMENT_SHADER,ShaderArc::fragment_default_shader.c_str());
    createShader(vertex_default,fagID,defaultShader);
    glDeleteShader(fagID);
    // Geometry
    fagID = genShader(GL_FRAGMENT_SHADER,ShaderArc::fragment_geometry_shader.c_str());
    createShader(vertex_default,fagID,geometryShader);
    glDeleteShader(fagID);
    // Font
    fagID = genShader(GL_FRAGMENT_SHADER,ShaderArc::fragment_font_default_shader.c_str());
    createShader(vertex_default,fagID,fontShader);
    glDeleteShader(fagID);

    //loadShaderSource("shader/tileMap.frag","shader/tileMap.vert",fragmentShaderSource,vertexShaderSource);
    //createShader(vertexShaderSource.c_str(), fragmentShaderSource.c_str(),tilemapShader);
    shaders.assign({defaultShader,geometryShader,fontShader});
    reload_resolution();
    useShader(0);
}

void reload_resolution(){
    for(GLuint &shader : shaders){
        glUseProgram(shader);
        glUniform4f(glGetUniformLocation(shader, "uColor"),1,1,1,1);
        glUniform2f(glGetUniformLocation(shader, "uResolution"),windowWidth,windowHeight);
    }
}

void setColor(float r,float g,float b,float a)
{
    for(GLuint &shader : shaders){
        glUseProgram(shader);
        GLint loc = glGetUniformLocation(shader, "uColor");
        if (loc != -1) glUniform4f(loc, r, g, b, a);
    }
}

void resize(int width, int height){
    if (width % 2 != 0) width ++;
    if (height % 2 != 0) height ++;
    glViewport(0,0,width,height);
    windowWidth  = width;
    windowHeight = height;
    reload_resolution();
}

void setRunTime(float rt)
{
    runTime = rt;
}

bool has_texture(std::string path)
{
    for(Texture &tex : textures){
        if(tex.path == path){
            return true;
        }
    }
    return false;
}

void get_textureSize(unsigned int ID ,int& width, int& height)
{
    Texture texInfo = textures[ID];
    width  = texInfo.width;
    height = texInfo.height;
}

unsigned int createTexture_data(unsigned char* data,int width,int height,int channel,int grid_x, int grid_y)
{
    if(grid_x < 0)
        grid_x = width;
    if(grid_y < 0)
        grid_y = height;
    
    if(data == nullptr)
        throw(std::string("Failed to load texture "));
    
    GLuint textureID;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    auto format = glFormats[channel];
    glTexImage2D(GL_TEXTURE_2D, 0, format.first, width, height, 0, format.second, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    Texture tex;
    tex.path      = "None";
    tex.textureID = textureID;
    tex.width     = width;
    tex.height    = height;
    tex.grid_x    = grid_x;
    tex.grid_y    = grid_y;

    textures.push_back(tex);
    return textures.size()-1;
}

unsigned int createTexture(const std::string &path,int grid_x, int grid_y)
{
    int width, height, channel;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channel, 0);

    if(grid_x <= 0)
        grid_x = width;
    if(grid_y <= 0)
        grid_y = height;
    
    if(data == nullptr)
        throw(std::string("Failed to load texture ") + path);
    
    GLuint textureID;
    
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    auto format = glFormats[channel];
    glTexImage2D(GL_TEXTURE_2D, 0, format.first, width, height, 0, format.second, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
    

    stbi_image_free(data);
    
    Texture tex;
    tex.path      = path;
    tex.textureID = textureID;
    tex.width     = width;
    tex.height    = height;
    tex.grid_x    = grid_x;
    tex.grid_y    = grid_y;

    textures.push_back(tex);
    return textures.size()-1;
}

void add_quad(float x,float y,float w,float h,float angle)
{
    glm::vec2 frameSize   = {w,h};
    glm::vec2 frameOffset = {0,0};
    draw(0,x,y,w,h,1,1,angle,frameOffset,frameSize);
}


void add_sprite(unsigned int id,float x,float y,float scale_x,float scale_y,float angle, int frame){
    Texture textureI  = textures[id];
    float w    = textureI.width;
    float h    = textureI.height;
    
    glm::vec2 frameSize = {textureI.grid_x,textureI.grid_y};

    int cols = w / textureI.grid_x;
    int rows = h / textureI.grid_y;

    int col = frame % cols;
    int row = frame / cols;

    glm::vec2 frameOffset = {col * frameSize.x,row * frameSize.y};
    
    draw(textureI.textureID,x,y,w,h,scale_x,scale_y,angle,frameOffset,frameSize,6,VAO_default);
}

void createVAOnVBO(GLuint& vao, GLuint& vbo)
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

void setUniformInt(const std::string &name, int value){
    glUniform1i(glGetUniformLocation(currentShader, name.c_str()), value);
}
void setUniformVec2(const std::string &name, float x, float y){
    glUniform2f(glGetUniformLocation(currentShader, name.c_str()),x,y);
}
void setUniformVec3(const std::string &name, float x, float y,float z){
    glUniform3f(glGetUniformLocation(currentShader, name.c_str()),x,y,z);
}
void setUniformVec4(const std::string &name, float x, float y, float z, float w){
    glUniform4f(glGetUniformLocation(currentShader, name.c_str()),x,y,z,w);
}
void setUniformFloat(const std::string &name, float angle){
    glUniform1f(glGetUniformLocation(currentShader, name.c_str()),angle);
}

//void useShader(unsigned int ID);

GLuint genShader(GLenum type,const char* shaderCode){
    unsigned int shader;
    char infoLog[512];
    int success;
    shader = glCreateShader(type);
    glShaderSource(shader, 1, &shaderCode, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "Shader Error: " << infoLog << std::endl;
        return 0;
    }
    return shader;
}

void createShader(unsigned int ver, unsigned int frag, unsigned int& ShaderID) {
    char infoLog[512];
    int success;
     
    // Shader Program
    ShaderID = glCreateProgram();
    glAttachShader(ShaderID, ver);
    glAttachShader(ShaderID, frag);
    glLinkProgram(ShaderID);
    glGetProgramiv(ShaderID, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(ShaderID, 512, NULL, infoLog);
        std::cout << "Shader Program Linking Error: " << infoLog << std::endl;
    }
}

void clean()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void useShader(unsigned int ID)
{
    GLuint id = shaders[ID];
    currentShader = id;
    glUseProgram(id);
}

void draw(GLuint textureID,float x, float y, float w, float h,float scale_x,float scale_y,float angle,glm::vec2 frameOffset,glm::vec2 frameSize,GLuint vertexCount,GLuint VAO) {
    glBindTexture(GL_TEXTURE_2D, textureID);
    glBindVertexArray(VAO);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    setUniformVec2 ("uTextureSize",w,h);
    setUniformVec2 ("uPos",x - (float)windowWidth/2.0f,-y + (float)windowHeight/2.0f);
    setUniformVec2 ("uScale",scale_x,scale_y);
    setUniformFloat("uTime",runTime);
    setUniformFloat("uAngle",angle);
    setUniformVec2 ("uFrameOffset",frameOffset.x,frameOffset.y);
    setUniformVec2 ("uFrameSize",frameSize.x,frameSize.y);

    glDrawElements(GL_TRIANGLES,vertexCount,GL_UNSIGNED_INT,0);
}
}