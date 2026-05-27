#include <cstdio>
#include <stb_image.h>
#include <variant>
#include "texture.h"
#include "texture_region.h"
#include "delog.h"
#include "window.h"
#include "renderer.h"
#include "shader_archive.h"
#include "helper.h"
#include "file.h"
#include "color.h"
#include "vertex.h"

constexpr int MAX_SPRITES  = 6000;
constexpr int MAX_VERTICES = MAX_SPRITES * 4;
constexpr int MAX_INDICES  = MAX_SPRITES * 6;

using UniformValue = std::variant<int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat4>;

GLFWwindow* window;
float runTime;
float zoom = 1.0f;

std::unordered_map<int, std::pair<GLint, GLenum>> glFormats = {
    {1, {GL_RED,  GL_RED}},
    {2, {GL_RG,   GL_RG}},
    {3, {GL_RGB,  GL_RGB}},
    {4, {GL_RGBA, GL_RGBA}},
};

GLuint vertex_default;
std::vector<GLuint> shaders;
unsigned int targetShader = 3;
unsigned int spriteShader;
unsigned int geometryShader;
unsigned int fontShader;
unsigned int default3DShader;
unsigned int tilemapShader;
unsigned int fontTexture;
unsigned int shader_target = 3;

Font defaultFont;
GLuint defaultShader  = 0;

Color  currentColor   = {1.0, 1.0, 1.0, 1.0};
GLuint currentShader  = 0;
GLuint currentTexture = 0;

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
    GLuint shader;
    std::string name;
    UniformValue value;
};

struct UniformLocation {
    GLuint shader;
    std::string name;
    GLint location;
};

struct RenderQueue2D
{
    GLuint textureID;
    GLuint shaderID;
    std::vector<Uniform> uniforms;

    unsigned int vertexOffset;
    unsigned int vertexCount;
};

std::vector<RenderQueue2D> renderQueue2DList;
std::vector<Vertex2D> vertex2DBuffer;

bool breakQueue2D = false;

namespace Render
{

int windowWidth, windowHeight;

GLuint loadShaderCode(GLenum type,const char* shaderCode);
unsigned int createShader(unsigned int ver, unsigned int frag);


bool batching = false;

GLuint batchVAO;
GLuint batchVBO;
GLuint batchEBO;

GLuint modelVAO;
GLuint modelVBO;
GLuint modelEBO;

std::vector<Uniform> uniformCache;
std::vector<UniformLocation> uniformLocationCache;

void init() {
    // --------------------------------  window  -------------------------------- //
    glfwGetWindowSize(Window::window, &windowWidth, &windowHeight);
    
    // -------------------------------- batching -------------------------------- //
    glGenVertexArrays(1, &batchVAO);
    glGenBuffers(1, &batchVBO);
    glGenBuffers(1, &batchEBO);
    glBindVertexArray(batchVAO);
    glBindBuffer(GL_ARRAY_BUFFER, batchVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batchEBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        MAX_VERTICES * sizeof(Vertex2D),
        nullptr,
        GL_DYNAMIC_DRAW
    );

    vertex2DBuffer.reserve(MAX_VERTICES * 5);

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
        sizeof(Vertex2D),
        (void*)0
    );
    glEnableVertexAttribArray(0);

    // UV
    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex2D),
        (void*)(2 * sizeof(float))
    );
    glEnableVertexAttribArray(1);

    // color
    glVertexAttribPointer(
        2,
        4,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex2D),
        (void*)(4 * sizeof(float))
        );
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // ------------------------------- load default shaders -------------------------------- //
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

    GLuint vertex_default_3d = loadShaderCode(GL_VERTEX_SHADER, ShaderArc::vertex_default_3D_shader);
    
    fagID = loadShaderCode(GL_FRAGMENT_SHADER, ShaderArc::fragment_default_3D_shader);
    default3DShader = createShader(vertex_default_3d, fagID);

    shaders.assign({spriteShader, geometryShader, fontShader, default3DShader});
    // ------------------------------------------------------------------------------------- //
    
    currentColor = {1.0, 1.0, 1.0, 1.0};
    defaultShader = spriteShader;
}

void beginBatch()
{
    batching = true;
}

void endBatch()
{
    batching = false;
    flush2D();
}

void setColor(float r, float g, float b, float a) {
    currentColor = {r, g, b, a};
}

void invalidateState() {
    breakQueue2D = true;
}


void setCachedUniform(const GLuint shader,const std::string& name, const UniformValue& value) {
    for (Uniform &uniform : uniformCache) {
        if (uniform.name == name && uniform.shader == shader) {
            uniform.value = value;
            return;
        }
    }
    uniformCache.push_back({shader,name,value});
}

GLint getUniformLocation(const GLuint shader, const std::string& name)
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

bool hasCachedUniformValue(const GLuint shader,const std::string& name, const UniformValue& value) {
    for (Uniform &uniform : uniformCache) {
        if (uniform.name     == name && 
            uniform.shader   == shader && 
            uniform.value    == value) 
        return true;
    }
    return false;
}

bool hasUniform(const GLuint shader,const std::string& name) {
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
    if (width  % 2 != 0) width++;
    if (height % 2 != 0) height++;
    windowWidth  = width;
    windowHeight = height;   
    glViewport(0, 0, width, height);
    for (GLuint &shader : shaders) {
        useShader(shader);
        setUniformVec2(shader, "uResolution", windowWidth, windowHeight);
    }
}

void setZoom(float value) {
    zoom = value;
}

unsigned int get_texture_id(const std::string &path) {
    return 0;
}

std::tuple<int,int> getWindowSize() {
    return {windowWidth, windowHeight};
}

void setViewTranslate(float x, float y) {
    view.x = x; view.y = y;
}

void setUniformBool(GLuint shader, std::string name, bool value) {
    setUniformInt(shader, name, static_cast<int>(value));
}

void setUniformInt(GLuint shader, const std::string &name, int value) {
    int location = getUniformLocation(shader, name);
    if (location == -1) return;
    UniformValue uv = value;
    if (hasCachedUniformValue(shader, name, uv)) return;
    setCachedUniform(shader, name, uv);
    breakQueue2D = true;
    glUniform1i(location, value);
}

void setUniformFloat(GLuint shader, const std::string &name, float value) {
    int location = getUniformLocation(shader, name);
    if (location == -1) return;
    UniformValue uv = value;
    if (hasCachedUniformValue(shader, name, uv)) return;
    setCachedUniform(shader, name, uv);
    breakQueue2D = true;
    glUniform1f(location, value);
}

void setUniformVec2(GLuint shader, const std::string &name, float x, float y) {
    int location = getUniformLocation(shader, name);
    if (location == -1) return;
    UniformValue uv = glm::vec2(x, y);
    if (hasCachedUniformValue(shader, name, uv)) return;
    setCachedUniform(shader, name, uv);
    breakQueue2D = true;
    glUniform2f(location, x, y);
}

void setUniformVec3(GLuint shader, const std::string &name, float x, float y, float z) {
    int location = getUniformLocation(shader, name);
    if (location == -1) return;
    UniformValue uv = glm::vec3(x, y, z);
    if (hasCachedUniformValue(shader, name, uv)) return;
    setCachedUniform(shader, name, uv);
    breakQueue2D = true;
    glUniform3f(location, x, y, z);
}

void setUniformVec4(GLuint shader, const std::string &name, float x, float y, float z, float w) {
    int location = getUniformLocation(shader, name);
    if (location == -1) return;
    UniformValue uv = glm::vec4(x, y, z, w);
    if (hasCachedUniformValue(shader, name, uv)) return;
    setCachedUniform(shader, name, uv);
    breakQueue2D = true;
    glUniform4f(location, x, y, z, w);
}

void setUniformMat4(GLuint shader, const std::string &name, const glm::mat4 &value) {
    int location = getUniformLocation(shader, name);
    if (location == -1) return;
    if (hasCachedUniformValue(shader, name, value)) return;
    setCachedUniform(shader, name, value);
    breakQueue2D = true;
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
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
        Delog::error("[%s] Shader Compile Error:\n%s", typeStr, infoLog);
        return 0;
    }
    const char* typeStr = (type == GL_VERTEX_SHADER) ? "VERTEX" : 
                          (type == GL_FRAGMENT_SHADER) ? "FRAGMENT" : "UNKNOWN";
    Delog::msg("[%s] Shader compiled successfully", typeStr);
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
        Delog::error("[PROGRAM] Shader Linking Error:\n%s", infoLog);
        glDeleteProgram(ShaderID);
        return 0;
    }

    Delog::msg("[PROGRAM] Shader linked successfully");
    return ShaderID;
}

Texture createTexture(uint8_t* data ,int width, int height, int channel) 
{
    GLuint textureID;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    GLenum format = GL_RGB;

    switch (channel)
    {
        case 1: format = GL_RED;  break;
        case 2: format = GL_RG;   break;
        case 3: format = GL_RGB;  break;
        case 4: format = GL_RGBA; break;
    }

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        format,
        width,
        height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        data
    );

    if (channel == 1)
    {
        GLint swizzleMask[] = {GL_ONE, GL_ONE, GL_ONE, GL_RED};
        glTexParameteriv(
            GL_TEXTURE_2D,
            GL_TEXTURE_SWIZZLE_RGBA,
            swizzleMask
        );
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);

    return Texture(textureID, width, height);
}

void useShader(unsigned int id) {
    if (currentShader == id) return;
    breakQueue2D = true;
    currentShader = id;
    glUseProgram(id);
}

void useTexture(unsigned int id) {
    if (currentTexture == id) return;
    breakQueue2D = true;
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

Quad computeQuadVertices(float cx, float cy, float w, float h, float angle)
{
    Quad q;

    float hw = w * 0.5f;
    float hh = h * 0.5f;

    if (angle == 0.0f)
    {
        q.x0 = cx - hw;  q.y0 = cy - hh;
        q.x1 = cx + hw;  q.y1 = cy - hh;
        q.x2 = cx + hw;  q.y2 = cy + hh;
        q.x3 = cx - hw;  q.y3 = cy + hh;
        return q;
    }

    float c = cosf(angle);
    float s = sinf(angle);

    float rotX_cos = hw * c;
    float rotX_sin = hw * s;
    float rotY_cos = hh * c;
    float rotY_sin = hh * s;

    q.x0 = cx - rotX_cos + rotY_sin;
    q.y0 = cy - rotX_sin - rotY_cos;

    q.x1 = cx + rotX_cos + rotY_sin;
    q.y1 = cy + rotX_sin - rotY_cos;

    q.x2 = cx + rotX_cos - rotY_sin;
    q.y2 = cy + rotX_sin + rotY_cos;

    q.x3 = cx - rotX_cos - rotY_sin;
    q.y3 = cy - rotX_sin + rotY_cos;

    return q;
}
void updateRenderQueue2D(unsigned int size)
{
    unsigned int vertexOffset = vertex2DBuffer.size();
    unsigned int vertexCount  = size;

    if (renderQueue2DList.empty()) {
        renderQueue2DList.push_back({currentTexture, currentShader, uniformCache, 0, vertexCount});
    } 
    else {
        auto& lastBatch = renderQueue2DList.back();

        if (!breakQueue2D) {
            lastBatch.vertexCount += vertexCount;
        } 
        else {
            unsigned int vertexOffset = vertex2DBuffer.size();
            unsigned int vertexCount  = size;
            renderQueue2DList.push_back({currentTexture, currentShader, uniformCache, vertexOffset, vertexCount});
            breakQueue2D = false;
        }
    }
}

void submitSprite(GLuint textureID, float x, float y, float tw, float th,float ox, float oy, float ow, float oh, float scale_x, float scale_y, float angle)
{
    if (!batching) return;
    
    useTexture(textureID);

    float u0 = ox / tw;
    float v0 = oy / th;
    float u1 = (ox + ow) / tw;
    float v1 = v0;
    float u2 = u1;
    float v2 = (oy + oh) / th;
    float u3 = u0;
    float v3 = v2;

    Quad q = computeQuadVertices(x, y, ow * scale_x, oh * scale_y, angle);
    Vertex2D ver0 = {q.x0, q.y0, u0, v0, currentColor.r, currentColor.g, currentColor.b, currentColor.a};
    Vertex2D ver1 = {q.x1, q.y1, u1, v1, currentColor.r, currentColor.g, currentColor.b, currentColor.a};
    Vertex2D ver2 = {q.x2, q.y2, u2, v2, currentColor.r, currentColor.g, currentColor.b, currentColor.a};
    Vertex2D ver3 = {q.x3, q.y3, u3, v3, currentColor.r, currentColor.g, currentColor.b, currentColor.a};
    
    updateRenderQueue2D(4);
    vertex2DBuffer.push_back(ver0);
    vertex2DBuffer.push_back(ver1);
    vertex2DBuffer.push_back(ver2);
    vertex2DBuffer.push_back(ver3);
}

void submitVertices(GLuint texture_id, const std::vector<Vertex2D>& vertices)
{
    if (!batching) return;
    useTexture(texture_id);
    
    vertex2DBuffer.insert(vertex2DBuffer.end(), vertices.begin(), vertices.end());
}

void applyUniforms(const std::vector<Uniform>& uniforms) {

}

void flush2D()
{
    if (vertex2DBuffer.empty()) return;
    glBindVertexArray(batchVAO);
    glBindBuffer(GL_ARRAY_BUFFER, batchVBO);

    for (size_t i = 0; i < renderQueue2DList.size(); i++) {
        RenderQueue2D& renderQueue2D = renderQueue2DList[i];
        
        useTexture(renderQueue2D.textureID);
        useShader(renderQueue2D.shaderID);
        applyUniforms(renderQueue2D.uniforms);

        unsigned int verticesLeft = renderQueue2D.vertexCount;
        unsigned int currentVertexOffset = renderQueue2D.vertexOffset;

        while (verticesLeft > 0) {
            unsigned int batchVerticesCount = std::min(verticesLeft, (unsigned int)MAX_VERTICES);
            unsigned int batchIndicesCount = (batchVerticesCount / 4) * 6;

            glBufferSubData(
                GL_ARRAY_BUFFER,
                0,
                batchVerticesCount * sizeof(Vertex2D),
                &vertex2DBuffer[currentVertexOffset]
            );

            glDrawElements(GL_TRIANGLES, batchIndicesCount, GL_UNSIGNED_INT, 0);

            verticesLeft -= batchVerticesCount;
            currentVertexOffset += batchVerticesCount;
        }
    }

    vertex2DBuffer.clear();
    renderQueue2DList.clear();
    glBindVertexArray(0);
}

void setFont(Font font) {
    defaultFont = font;
}

void drawText(const std::string& text,float x,float y,const std::string& align)
{
    useShader(spriteShader);
    const Texture& texture = defaultFont.getTexture();
    
    std::vector<uint32_t> codepoints = string2U32(text);
    std::vector<Vertex2D> vertices;
    std::vector<float> lineAdvanceWidths;
    
    float lineAdvanceWidth = 0.0f;
    float lineHeight = defaultFont.getLineHeight();

    vertices.reserve(codepoints.size() * 4);
    lineAdvanceWidths.push_back(0.0f);

    int align_mode = LEFT_AlIGN;
    if (align == "center") 
        align_mode = CENTER_ALIGN;
    if (align == "right") 
        align_mode = RIGHT_ALIGN;

    for (uint32_t c : codepoints)
    {
        if (c == '\n')
        {
            lineAdvanceWidths.push_back(0.0f);
            lineAdvanceWidth = 0.0f;
            continue;
        }
        const Glyph& glyph = defaultFont.getGlyph(c);
        lineAdvanceWidths.back() += glyph.advance;
    }

    float cx = 0.0f;
    float cy = 0.0f;

    int lineIndex = 0;

    for (uint32_t c : codepoints)
    {
        if (c == '\n')
        {
            cx = 0.0f;
            cy += lineHeight;
            lineIndex++;
            continue;
        }

        const Glyph& glyph = defaultFont.getGlyph(c);
        float shift = 0.0f;

        if (align_mode == CENTER_ALIGN)
            shift = -lineAdvanceWidths[lineIndex] * 0.5f;
        else if (align_mode == RIGHT_ALIGN)
            shift = -lineAdvanceWidths[lineIndex];

        float x0 = x + cx + glyph.offset_x + shift;
        float y0 = y + cy - glyph.offset_y + lineHeight;

        float x1 = x0 + glyph.width;
        float y1 = y0;
        float x2 = x1;
        float y2 = y0 + glyph.height;
        float x3 = x0;
        float y3 = y2;

        float u0 = glyph.u0; 
        float v0 = glyph.v0; 
        float u1 = glyph.u1; 
        float v1 = glyph.v1;
        float u2 = glyph.u2; 
        float v2 = glyph.v2; 
        float u3 = glyph.u3; 
        float v3 = glyph.v3;

        float r = currentColor.r;
        float g = currentColor.g;
        float b = currentColor.b;
        float a = currentColor.a;

        Vertex2D ver0 = {x0, y0, u0, v0, r, g, b, a};
        Vertex2D ver1 = {x1, y1, u1, v1, r, g, b, a};
        Vertex2D ver2 = {x2, y2, u2, v2, r, g, b, a};
        Vertex2D ver3 = {x3, y3, u3, v3, r, g, b, a};

        vertices.push_back(ver0);
        vertices.push_back(ver1);
        vertices.push_back(ver2);
        vertices.push_back(ver3);

        cx += glyph.advance + defaultFont.getLetterSpacing();
    }
    
    submitVertices(1, vertices);
}

void createTilemapMesh(int ID, const std::vector<int>& tiles, int W, int H, int Row, int Col, int pixelSize) {
}

void drawSprite(TextureRegion& texture_region, float x, float y, float angle, float scale_x, float scale_y) {
    unsigned int textureID = texture_region.get_texture().getID();
    int tw = texture_region.get_texture().getWidth();
    int th = texture_region.get_texture().getHeight();
    int ox = texture_region.x;
    int oy = texture_region.y;
    int ow = texture_region.w;
    int oh = texture_region.h;
    useShader(defaultShader);
    submitSprite(textureID, x, y, tw, th, ox, oy, ow, oh, scale_x, scale_y, angle);
}

void drawRectangle(float x, float y, float w, float h, bool fill) {
    useShader(geometryShader);
    setUniformInt(geometryShader, "uGeometry", fill ? 0 : 2);
    setUniformVec2(geometryShader, "uTextureSize", w, h);
    submitSprite(currentTexture, int(x), int(y), int(w), int(h), 0, 0, int(w), int(h), 1, 1, 0);
}


void drawCircle(float x, float y, float r, bool fill){
    float w = r * 2;
    float h = r * 2;
    useShader(geometryShader);
    setUniformInt(geometryShader, "uGeometry", fill ? 1 : 3);
    setUniformVec2(geometryShader, "uTextureSize", w, h);
    submitSprite(currentTexture, int(x), int(y), int(w), int(h), 0, 0, int(w), int(h), 1, 1, 0);
}

void drawMesh(Mesh* mesh){
    useShader(default3DShader);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    float aspect = (float)windowWidth / (float)windowHeight;
    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),
        aspect,
        0.1f,
        100.0f
    );
    
    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 8.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    
    glm::mat4 model = glm::mat4(1.0f);
    
    glm::mat4 MVP = projection * view * model;
   
    setUniformMat4(default3DShader, "uMVP", MVP);
    
    glBindVertexArray(mesh->primitives[0].VAO);
    glDrawElements(GL_TRIANGLES, mesh->primitives[0].indices.size(), GL_UNSIGNED_INT, 0);

    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
}

void setShader(unsigned int id) {
    defaultShader = shaders[id];
}

}
