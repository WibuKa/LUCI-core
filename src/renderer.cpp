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

constexpr int MAX_SPRITES  = 5000;
constexpr int MAX_VERTICES = MAX_SPRITES * 4;
constexpr int MAX_INDICES  = MAX_SPRITES * 6;

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

std::unordered_map<std::string,unsigned int> pathCache;

struct View
{
    float x = 0;
    float y = 0;
    float z = 0;
    float rotate = 0;
    float scale = 1;
} view;

struct Uniform
{
    unsigned int shader;
    std::string name;
    int type;
    float value[4];
};

struct UniformLocation {
    GLuint shader;
    std::string name;
    GLint location;
};

namespace Render
{

int windowWidth, windowHeight;

GLuint loadShaderCode(GLenum type,const char* shaderCode);
unsigned int createShader(unsigned int ver, unsigned int frag);

std::vector<Vertex> batchVertices;
std::vector<Instance> batchInstances;

bool batching = false;

GLuint batchVAO;
GLuint batchVBO;
GLuint batchEBO;

std::vector<Uniform> uniformCache;
std::vector<UniformLocation> uniformLocationCache;

void init(GLFWwindow* glwd) {
    // --------------------------------  window  -------------------------------- //
    glfwGetWindowSize(glwd, &windowWidth, &windowHeight);
    
    // -------------------------------- batching -------------------------------- //
    glGenVertexArrays(1, &batchVAO);
    glGenBuffers(1, &batchVBO);
    glGenBuffers(1, &batchEBO);

    glBindVertexArray(batchVAO);
    glBindBuffer(GL_ARRAY_BUFFER, batchVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batchEBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        MAX_VERTICES * sizeof(Vertex),
        nullptr,
        GL_DYNAMIC_DRAW
    );

    std::vector<unsigned int> staticIndices;

    staticIndices.reserve(MAX_INDICES);

    for (unsigned int i = 0; i < MAX_SPRITES; i++)
    {
        unsigned int offset = i * 4;

        staticIndices.push_back(offset + 0);
        staticIndices.push_back(offset + 1);
        staticIndices.push_back(offset + 3);

        staticIndices.push_back(offset + 1);
        staticIndices.push_back(offset + 2);
        staticIndices.push_back(offset + 3);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batchEBO);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        staticIndices.size() * sizeof(unsigned int),
        staticIndices.data(),
        GL_STATIC_DRAW
    );

    // position
    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)0
    );
    glEnableVertexAttribArray(0);

    // UV
    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)(2 * sizeof(float))
    );
    glEnableVertexAttribArray(1);

    // color
    glVertexAttribPointer(
        2,
        4,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)(4 * sizeof(float))
    );
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    batchVertices.reserve(MAX_VERTICES);
    batchInstances.reserve(MAX_VERTICES);

    // ------------------------------------------------------------------------------ //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLuint fagID;
    vertex_default = loadShaderCode(GL_VERTEX_SHADER, ShaderArc::vertex_default_shader);

    fagID = loadShaderCode(GL_FRAGMENT_SHADER, ShaderArc::fragment_default_shader);
    spriteShader = createShader(vertex_default, fagID);
    glDeleteShader(fagID);

    fagID = loadShaderCode(GL_FRAGMENT_SHADER, ShaderArc::fragment_geometry_shader);
    geometryShader = createShader(vertex_default, fagID);
    glDeleteShader(fagID);

    fagID = loadShaderCode(GL_FRAGMENT_SHADER, ShaderArc::fragment_font_default_shader);
    fontShader = createShader(vertex_default, fagID);
    glDeleteShader(fagID);

    shaders.assign({spriteShader, geometryShader, fontShader});
    
    currentColor = {1.0, 1.0, 1.0, 1.0};
    font_default_id = FontSYS::load_font("FVF Fernando 08.ttf", 15);
    font_id = font_default_id;
    defaultShader = spriteShader;
}

void beginBatch()
{
    batching = true;
}

void endBatch()
{
    batching = false;
}

void setColor(float r, float g, float b, float a) {
    currentColor = {r, g, b, a};
}

void setCachedUniform(unsigned int shader, std::string name,int type,float x, float y, float z, float w) {
    for (Uniform &uniform : uniformCache) {
        if (uniform.name == name && uniform.shader == shader && uniform.type == type) {
            uniform.value[0] = x;
            uniform.value[1] = y;
            uniform.value[2] = z;
            uniform.value[3] = w;
            return;
        }
    }
    uniformCache.push_back({shader,name,type,x,y,z,w});
}

GLint getUniformLocation(GLuint shader, const std::string& name)
{
    for (const auto& uniform : uniformLocationCache){
        if (uniform.shader == shader && uniform.name == name){
            return uniform.location;
        }
    }
    GLint location = glGetUniformLocation(shader, name.c_str());
    uniformLocationCache.push_back({shader,name,location});
    return location;
}

bool hasCachedUniformValue(int shader, std::string name, int type, float x, float y, float z, float w) {
    for (Uniform &uniform : uniformCache) {
        if (uniform.name     == name && 
            uniform.shader   == shader && 
            uniform.type     == type &&
            uniform.value[0] == x &&
            uniform.value[1] == y &&
            uniform.value[2] == z &&
            uniform.value[3] == w
        ) return true;
    }
    return false;
}

bool hasUniform(int shader, std::string name) {
    if (getUniformLocation(shader, name) == -1) return false;
    return true;
}

void setTime(float rt) {
    for (GLuint &shader : shaders) {
        if (hasUniform(shader, "uTime")) {
            useShader(shader);
            setUniformFloat(shader, "uTime",rt);
        }
    }
}

void resize(int width, int height) {
    if (width % 2 != 0) width++;
    if (height % 2 != 0) height++;
    glViewport(0, 0, width, height);
    windowWidth  = width;
    windowHeight = height;
    for (GLuint &shader : shaders) {
        useShader(shader);
        setUniformVec2(shader, "uResolution", windowWidth, windowHeight);
    }
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

void setViewTranslate(float x, float y) {
    view.x = x; view.y = y;
}

void setUniformBool(GLuint shader, std::string name, bool value) {
    if (hasCachedUniformValue(shader, name, UNIFORM_INT, value, 0, 0, 0)) return;
    setCachedUniform(shader, name, UNIFORM_INT, value, 0, 0, 0);
    setUniformInt(shader, name, value);
}

void setUniformInt(GLuint shader, const std::string &name, int value) {
    if (hasCachedUniformValue(shader, name, UNIFORM_INT, value, 0, 0, 0)) return;
    int location = getUniformLocation(shader, name);
    if (location == -1) return;
    setCachedUniform(shader, name, UNIFORM_INT, value, 0, 0, 0);
    glUniform1i(location, value);
}

void setUniformFloat(GLuint shader, const std::string &name, float value) {
    if (hasCachedUniformValue(shader, name, UNIFORM_FLOAT, value, 0, 0, 0)) return;
    int location = getUniformLocation(shader, name);
    if (location == -1) return;
    setCachedUniform(shader, name, UNIFORM_FLOAT, value, 0, 0, 0);
    glUniform1f(location, value);
}

void setUniformVec2(GLuint shader, const std::string &name, float x, float y) {
    if (hasCachedUniformValue(shader, name, UNIFORM_VEC2, x, y, 0, 0)) return;
    int location = getUniformLocation(shader, name);
    if (location == -1) return;
    setCachedUniform(shader, name, UNIFORM_VEC2, x, y, 0, 0);
    glUniform2f(location, x, y);
}

void setUniformVec3(GLuint shader, const std::string &name, float x, float y, float z) {
    if (hasCachedUniformValue(shader, name, UNIFORM_VEC3, x, y, z, 0)) return;
    int location = getUniformLocation(shader, name);
    if (location == -1) return;
    setCachedUniform(shader, name, UNIFORM_VEC3, x, y, z, 0);
    glUniform3f(location, x, y, z);
}

void setUniformVec4(GLuint shader, const std::string &name, float x, float y, float z, float w) {
    if (hasCachedUniformValue(shader, name, UNIFORM_VEC4, x, y, z, w)) return;
    int location = getUniformLocation(shader, name);
    if (location == -1) return;
    setCachedUniform(shader, name, UNIFORM_VEC4, x, y, z, w);
    glUniform4f(location, x, y, z, w);
}


GLuint loadShaderCode(GLenum type, const char* shaderCode) {
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
    flush();
    currentShader = id;
    glUseProgram(id);
}

void useTexture(unsigned int id) {
    if (currentTexture == id) return;
    flush();
    currentTexture = id;
    glBindTexture(GL_TEXTURE_2D, id);
}

void pushShaderID(unsigned int ShaderID) {
    useShader(ShaderID);
}

unsigned int loadShader(std::string shader_Path) {
    std::string shaderCode = File::read(shader_Path);
    GLuint fragID = loadShaderCode(GL_FRAGMENT_SHADER, shaderCode.c_str());
    unsigned int result = shaders.size();
    GLuint program = createShader(vertex_default, fragID);
    glDeleteShader(fragID);
    shaders.push_back(program);
    useShader(program);
    return result;
}

Quad makeQuad(float cx, float cy, float w, float h, float angle) {
    float hw = w  * 0.5f;
    float hh = h * 0.5f;

    // local vertices
    float lx0 = -hw;
    float ly0 = -hh;

    float lx1 =  hw;
    float ly1 = -hh;

    float lx2 =  hw;
    float ly2 =  hh;

    float lx3 = -hw;
    float ly3 =  hh;

    // rotation
    float c = cos(angle);
    float s = sin(angle);

    Quad q;

    // vertex 0
    q.x0 = lx0 * c - ly0 * s + cx;
    q.y0 = lx0 * s + ly0 * c + cy;

    // vertex 1
    q.x1 = lx1 * c - ly1 * s + cx;
    q.y1 = lx1 * s + ly1 * c + cy;

    // vertex 2
    q.x2 = lx2 * c - ly2 * s + cx;
    q.y2 = lx2 * s + ly2 * c + cy;

    // vertex 3
    q.x3 = lx3 * c - ly3 * s + cx;
    q.y3 = lx3 * s + ly3 * c + cy;

    return q;
}

void submitSprite(GLuint textureID, float x, float y, float tw, float th,float ox, float oy, float ow, float oh, float scale_x, float scale_y, float angle)
{
    if (!batching) return;
    if (batchVertices.size() + 4 > MAX_VERTICES) flush();

    useTexture(textureID);

    float x0 = -0.5;
    float y0 = -0.5;
    float x1 =  0.5;
    float y1 = -0.5;
    float x2 =  0.5;
    float y2 =  0.5;
    float x3 = -0.5;
    float y3 =  0.5;

    float u0 = ox / tw;
    float v0 = oy / th;
    float u1 = (ox + ow) / tw;
    float v1 = oy / th;
    float u2 = (ox + ow) / tw;
    float v2 = (oy + oh) / th;
    float u3 = ox / tw;
    float v3 = (oy + oh) / th;

    batchInstances.push_back({x, y, angle, scale_x, scale_y, currentColor.r, currentColor.g, currentColor.b, currentColor.a});

    Quad q = makeQuad(x, y, ow * scale_x, oh * scale_y, angle);
    Vertex ver0 = {q.x0, q.y0, u0, v0, currentColor.r, currentColor.g, currentColor.b, currentColor.a};
    Vertex ver1 = {q.x1, q.y1, u1, v1, currentColor.r, currentColor.g, currentColor.b, currentColor.a};
    Vertex ver2 = {q.x2, q.y2, u2, v2, currentColor.r, currentColor.g, currentColor.b, currentColor.a};
    Vertex ver3 = {q.x3, q.y3, u3, v3, currentColor.r, currentColor.g, currentColor.b, currentColor.a};

    batchVertices.push_back(ver0);
    batchVertices.push_back(ver1);
    batchVertices.push_back(ver2);
    batchVertices.push_back(ver3);

}

void flush()
{
    if(batchVertices.empty()) return;

    glBindVertexArray(batchVAO);
    glBindBuffer(GL_ARRAY_BUFFER, batchVBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        MAX_VERTICES * sizeof(Vertex),
        nullptr,
        GL_DYNAMIC_DRAW
    );

    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        batchVertices.size() * sizeof(Vertex),
        batchVertices.data()
    );

    glDrawElements(
        GL_TRIANGLES,
        (batchVertices.size() / 4) * 6,
        GL_UNSIGNED_INT,
        0
    );

    batchVertices.clear();

    glBindVertexArray(0);
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

void drawSprite(TextureRegion& texture_region, float x, float y, float angle, float scale_x, float scale_y) {
    unsigned int textureID = texture_region.get_texture()->get_id();
    int tw = texture_region.get_texture()->get_width();
    int th = texture_region.get_texture()->get_height();
    int ox = texture_region.x;
    int oy = texture_region.y;
    int ow = texture_region.w;
    int oh = texture_region.h;
    useShader(defaultShader);
    submitSprite(textureID, x, y, tw, th, ox, oy, ow, oh, scale_x, scale_y, angle);
    //draw(textureID, x, y, tw, th, ox, oy, ow, oh, scale_x, scale_y, angle, 6, VAO_default);
}

void drawRectangle(float x, float y, float w, float h, bool fill) {
    useShader(geometryShader);
    setUniformInt(geometryShader, "uGeometry", fill ? 0 : 2);
    setUniformVec2(geometryShader, "uTextureSize", w, h);
    submitSprite(currentTexture, int(x), int(y), int(w), int(h), 0, 0, int(w), int(h), 1, 1, 0);
    flush();
}


void drawCircle(float x, float y, float r, bool fill){
    float w = r * 2;
    float h = r * 2;
    useShader(geometryShader);
    setUniformInt(geometryShader, "uGeometry", fill ? 1 : 3);
    setUniformVec2(geometryShader, "uTextureSize", w, h);
    submitSprite(currentTexture, int(x), int(y), int(w), int(h), 0, 0, int(w), int(h), 1, 1, 0);
}

void setShader(unsigned int id) {
    defaultShader = shaders[id];
}

}
