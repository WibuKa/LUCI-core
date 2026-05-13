#define STB_IMAGE_IMPLEMENTATION
#include <cstdio>
#include <stb/stb_image.h>
#include "renderer.h"
#include "shader_archive.h"
#include "font.h"
#include "helper.h"
#include "file.h"
#include "color.h"
#include <iostream>

GLFWwindow* window;
float runTime;
float zoom = 1.0f;

std::unordered_map<int, std::pair<GLint, GLenum>> glFormats = {
    {1, {GL_RED,  GL_RED}},
    {2, {GL_RG,   GL_RG}},
    {3, {GL_RGB,  GL_RGB}},
    {4, {GL_RGBA, GL_RGBA}},
};

std::vector<RenderTexture> textures;
GLuint vertex_default;
std::vector<GLuint> shaders;
unsigned int targetShader = 3;
unsigned int spriteShader;
unsigned int geometryShader;
unsigned int fontShader;
unsigned int tilemapShader;
unsigned int fontTexture;
unsigned int shader_target = 3;

GLuint font_default_id;
GLuint font_id;

Color currentColor    = {1.0, 1.0, 1.0, 1.0};
GLuint currentShader  = 0;
GLuint currentTexture = 0;
GLuint defaultShader  = 0;
GLuint currentVAO     = 0;
GLuint VAO_default;

std::unordered_map<std::string,unsigned int> pathCache;

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

struct View
{
    float x = 0;
    float y = 0;
    float z = 0;
    float rotate = 0;
    float scale = 1;
} view;

namespace Render
{

int windowWidth, windowHeight;

GLuint genShader(GLenum type,const char* shaderCode);
unsigned int createShader(unsigned int ver, unsigned int frag);

void init(GLFWwindow* glwd) {
    window = glwd;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    GLuint VBO, EBO;
    glGenBuffers(1, &EBO);

    createVAOnVBO(VAO_default, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLuint fagID;
    vertex_default = genShader(GL_VERTEX_SHADER, ShaderArc::vertex_default_shader);

    fagID = genShader(GL_FRAGMENT_SHADER, ShaderArc::fragment_default_shader);
    spriteShader = createShader(vertex_default, fagID);
    glDeleteShader(fagID);

    fagID = genShader(GL_FRAGMENT_SHADER, ShaderArc::fragment_geometry_shader);
    geometryShader = createShader(vertex_default, fagID);
    glDeleteShader(fagID);

    fagID = genShader(GL_FRAGMENT_SHADER, ShaderArc::fragment_font_default_shader);
    fontShader = createShader(vertex_default, fagID);
    glDeleteShader(fagID);
    
    shaders.assign({spriteShader, geometryShader, fontShader});
    
    currentColor = {1.0, 1.0, 1.0, 1.0};
    setColor(currentColor.r, currentColor.g, currentColor.b, currentColor.a);
    font_default_id = FontSYS::load_font("FVF Fernando 08.ttf", 15);
    font_id = font_default_id;
    defaultShader = spriteShader;
}

void setColor(float r, float g, float b, float a) {
    currentColor = {r, g, b, a};
    for (GLuint &shader : shaders) {
        useShader(shader);
        setUniformVec4(shader, "uColor",r, g, b, a);
    }
}

void setTime(float rt) {
    for (GLuint &shader : shaders) {
        useShader(shader);
        setUniformFloat(shader, "uTime",rt);
    }
}

void resize(int width, int height) {
    if (width % 2 != 0) width++;
    if (height % 2 != 0) height++;
    glViewport(0, 0, width, height);
    windowWidth  = width;
    windowHeight = height;
}

void setZoom(float value) {
    zoom = value;
}

bool has_texture(std::string path) {
    for (RenderTexture &tex : textures) {
        if (tex.path == path) return true;
    }
    return false;
}

void get_textureSize(unsigned int ID, int& width, int& height) {
    RenderTexture texInfo = textures[ID];
    width  = texInfo.width;
    height = texInfo.height;
}

unsigned int createTexture_data(unsigned char* data, int width, int height, int channel, int grid_x, int grid_y) {
    if (grid_x < 0) grid_x = width;
    if (grid_y < 0) grid_y = height;

    if (data == nullptr) throw(std::string("Failed to load texture "));

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    auto format = glFormats[channel];
    glTexImage2D(GL_TEXTURE_2D, 0, format.first, width, height, 0, format.second, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);

    RenderTexture tex{"None", textureID, width, height, grid_x, grid_y};
    textures.push_back(tex);
    return textures.size() - 1;
}

unsigned int createTexture(const std::string &path, int grid_x, int grid_y) {
    int width, height, channel;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channel, 0);

    if (grid_x <= 0) grid_x = width;
    if (grid_y <= 0) grid_y = height;

    if (data == nullptr) throw(std::string("Failed to load texture ") + path);

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    auto format = glFormats[channel];
    glTexImage2D(GL_TEXTURE_2D, 0, format.first, width, height, 0, format.second, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);

    RenderTexture tex;
    tex.path      = path;
    tex.textureID = textureID;
    tex.width     = width;
    tex.height    = height;
    tex.grid_x    = grid_x;
    tex.grid_y    = grid_y;

    textures.push_back(tex);
    return textures.size() - 1;
}

unsigned int get_texture_id(const std::string &path) {
    return 0;
    //return load_texture(path, -1, -1);
}

std::tuple<int,int> getWindowSize() {
    return {windowWidth, windowHeight};
}

std::tuple<int,int> getTextureSize(unsigned id) {
    int width = 0, height = 0;
    if (id) get_textureSize(id, width, height);
    return {width, height};
}

void add_quad(float x, float y, float w, float h, float angle) {
    glm::vec2 frameSize   = {w, h};
    glm::vec2 frameOffset = {0, 0};
    //draw(0, x, y, w, h, 1, 1, angle, frameOffset, frameSize);
}

void add_sprite(unsigned int id, float x, float y, float scale_x, float scale_y, float angle, int frame) {
    RenderTexture textureI = textures[id];
    float w = textureI.width;
    float h = textureI.height;

    glm::vec2 frameSize = {textureI.grid_x, textureI.grid_y};
    int cols = w / textureI.grid_x;
    int rows = h / textureI.grid_y;
    int col = frame % cols;
    int row = frame / cols;

    glm::vec2 frameOffset = {col * frameSize.x, row * frameSize.y};
    //draw(textureI.textureID, x, y, w, h, scale_x, scale_y, angle, frameOffset, frameSize, 6, VAO_default);
}

void createVAOnVBO(GLuint& vao, GLuint& vbo) {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

void setViewTranslate(float x, float y) {
    view.x = x; view.y = y;
}

void setUniformInt(GLuint shader, const std::string &name, int value) {
    glUniform1i(glGetUniformLocation(shader, name.c_str()), value);
}

void setUniformVec2(GLuint shader, const std::string &name, float x, float y) {
    glUniform2f(glGetUniformLocation(shader, name.c_str()), x, y);
}

void setUniformVec3(GLuint shader, const std::string &name, float x, float y, float z) {
    glUniform3f(glGetUniformLocation(shader, name.c_str()), x, y, z);
}

void setUniformVec4(GLuint shader, const std::string &name, float x, float y, float z, float w) {
    glUniform4f(glGetUniformLocation(shader, name.c_str()), x, y, z, w);
}

void setUniformFloat(GLuint shader, const std::string &name, float value) {
    glUniform1f(glGetUniformLocation(shader, name.c_str()), value);
}

void setUniformBool(GLuint shader, std::string name, bool v) {
    setUniformInt(shader, name, v);
}

GLuint genShader(GLenum type, const char* shaderCode) {
    unsigned int shader;
    char infoLog[1024];
    int success;
    shader = glCreateShader(type);
    glShaderSource(shader, 1, &shaderCode, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        const char* typeStr = (type == GL_VERTEX_SHADER) ? "VERTEX" : 
                              (type == GL_FRAGMENT_SHADER) ? "FRAGMENT" : "UNKNOWN";
        std::cerr << "[" << typeStr << "] Shader Compile Error:\n" << infoLog << std::endl;
        return 0;
    }
    const char* typeStr = (type == GL_VERTEX_SHADER) ? "VERTEX" : 
                          (type == GL_FRAGMENT_SHADER) ? "FRAGMENT" : "UNKNOWN";
    std::cout << "[" << typeStr << "] Shader compiled successfully" << std::endl;
    return shader;
}

unsigned int createShader(unsigned int ver, unsigned int frag) {
    unsigned int ShaderID;
    char infoLog[1024];
    int success;

    ShaderID = glCreateProgram();

    glAttachShader(ShaderID, ver);
    glAttachShader(ShaderID, frag);
    glLinkProgram(ShaderID);
    glGetProgramiv(ShaderID, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(ShaderID, 1024, NULL, infoLog);
        std::cerr << "[PROGRAM] Shader Linking Error:\n"<< infoLog << std::endl;
        glDeleteProgram(ShaderID);
        return 0;
    }

    std::cout << "[PROGRAM] Shader linked successfully" << std::endl;
    return ShaderID;
}

void useShader(unsigned int id) {
    if (currentShader == id) return;
    currentShader = id;
    glUseProgram(id);
}

void pushShaderID(unsigned int ShaderID) {
    useShader(ShaderID);
}

unsigned int loadShader(std::string shader_Path) {
    std::string shaderCode = File::read(shader_Path);
    GLuint fragID = genShader(GL_FRAGMENT_SHADER, shaderCode.c_str());
    unsigned int result = shaders.size();
    GLuint program = createShader(vertex_default, fragID);
    glDeleteShader(fragID);
    shaders.push_back(program);
    useShader(program);
    setUniformVec4(program, "uColor", currentColor.r, currentColor.g, currentColor.b, currentColor.a);
    return result;
}

void draw(GLuint textureID, float x, float y, float tw, float th,int ox, int oy, int ow, int oh, float scale_x, float scale_y, float angle, GLuint vertexCount, GLuint VAO) {

    glBindTexture(GL_TEXTURE_2D, textureID);
    glBindVertexArray(VAO);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    setUniformVec2(currentShader, "uTextureSize", tw, th);
    setUniformVec2(currentShader, "uPos", x - (float)windowWidth/2.0f, -y + (float)windowHeight/2.0f);
    setUniformVec2(currentShader, "uScale", scale_x, scale_y);
    setUniformFloat(currentShader, "uAngle", angle);
    setUniformVec4(currentShader, "uRegion", ox, oy, ow, oh);
    setUniformVec2(currentShader, "uView", view.x, view.y);
    setUniformVec2(currentShader, "uResolution", windowWidth, windowHeight);
    
    glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 0);

}

void loadFragmentShaderSource(std::string path) {
}

void set_font(int id) {
    FontSYS::set_font(id);
}

int load_font(std::string path, int font_size) {
    if (font_size < 0) font_size = 0;
    return FontSYS::load_font(path, font_size);
}

void drawText(const std::string& text, float x, float y, const std::string& align) {
    useShader(fontShader);
    std::vector<uint32_t> unicodes = string2U32(text);

    struct Glyph {
        float x, y;
        float ox, oy;
        float w, h;
        int line;
    };

    std::vector<Glyph> glyphs;
    std::vector<float> lineWidths;

    float lineHeight = FontSYS::get_line_height();
    float cx = 0.0f, cy = 0.0f;
    uint32_t prev = 0;
    int lineIndex = 0;

    lineWidths.push_back(0.0f);

    for (uint32_t c : unicodes) {
        float ox, oy, w, h, asc, adv;
        FontSYS::get_glyph(c, ox, oy, w, h, asc, adv);

        if (c == '\n') {
            lineIndex++;
            lineWidths.push_back(0.0f);
            cx = 0;
            cy += lineHeight;
            prev = 0;
            continue;
        }

        float kern = FontSYS::get_kern(prev, c);

        Glyph g;
        g.x = x + cx + adv * 0.5f;
        g.y = y + cy + h * 0.5f - asc + lineHeight;
        g.ox = ox; g.oy = oy;
        g.w = w; g.h = h;
        g.line = lineIndex;

        glyphs.push_back(g);
        cx += adv + kern;
        lineWidths[lineIndex] = cx;
        prev = c;
    }

    std::vector<float> lineShift(lineWidths.size(), 0.0f);
    for (int i = 0; i < lineWidths.size(); i++) {
        if (align == "center") lineShift[i] = -lineWidths[i] * 0.5f;
        else if (align == "right") lineShift[i] = -lineWidths[i];
        else lineShift[i] = 0.0f;
    }

    for (const Glyph& g : glyphs) {
        float finalX = g.x + lineShift[g.line];
        //draw(1, finalX, g.y, 1024, 1024, 1, 1, 0, {g.ox, g.oy}, {g.w, g.h});
    }
}

void createTilemapMesh(int ID, const std::vector<int>& tiles, int W, int H, int Row, int Col, int pixelSize) {
}

void draw_sprite(TextureRegion& texture_region, float x, float y, float angle, float scale_x, float scale_y) {
    useShader(defaultShader);
    unsigned int textureID = texture_region.get_texture()->get_id();
    int tw = texture_region.get_texture()->get_width();
    int th = texture_region.get_texture()->get_height();
    int ox = texture_region.x;
    int oy = texture_region.y;
    int ow = texture_region.w;
    int oh = texture_region.h;

    draw(textureID, x, y, tw, th, ox, oy, ow, oh, scale_x, scale_y, angle, 6, VAO_default);
}

void draw_rectangle(float x, float y, float w, float h, bool fill) {
    useShader(geometryShader);
    setUniformInt(geometryShader, "geometry", fill ? 0 : 2);
    draw(0, x, y, w, h, 0, 0, w, h, 1, 1, 0, 6, VAO_default);
}


void drawCircle(float x, float y, float r, bool fill){
    useShader(geometryShader);
    setUniformInt(geometryShader, "geometry", fill ? 1 : 3);
    float w = r * 2;
    float h = r * 2;
    draw(0, x, y, w, h, 0, 0, w, h, 1, 1, 0, 6, VAO_default);
}

void setShader(unsigned int id) {
    defaultShader = shaders[id];
}

}
