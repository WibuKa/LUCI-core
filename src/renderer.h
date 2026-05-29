#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <tuple>
#include <color.h>
#include "texture_region.h"
#include "font.h"
#include "model.h"
#include "camera.h"

struct GLFWwindow;

struct Quad {
    float x0, y0;
    float x1, y1;
    float x2, y2;
    float x3, y3;
};

extern std::unordered_map<std::string,unsigned int> pathCache;

namespace Render {
    struct View
    {
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);

        int type = PERSPECTIVE_CAMERA;
        
        float fov = 45.0f;
        float near = 0.1f;
        float far = 100.0f;

        void applyCamera(Camera* camera)
        {
            if (camera->type == PERSPECTIVE_CAMERA)
            {
                type = PERSPECTIVE_CAMERA;
                fov = camera->fov;
                near = camera->near;
                far = camera->far;
            }
            else if (camera->type == ORTHOGRAPHIC_CAMERA)
            {
                type = ORTHOGRAPHIC_CAMERA;
                fov = 0.0f;
                near = camera->near;
                far = camera->far;
            }
        }

        glm::mat4 getMat4()
        {
            float pitch = glm::radians(rotation.x);
            float yaw   = glm::radians(rotation.y);

            glm::vec3 front;
            front.x = cos(pitch) * sin(yaw);
            front.y = sin(pitch);
            front.z = cos(pitch) * cos(yaw);
            front = glm::normalize(front);

            return glm::lookAt(position, position + front, glm::vec3(0.0f, 1.0f, 0.0f));
        }
    };

    extern int windowWidth, windowHeight;
    extern View view;

    void init();
    void resize(int width, int height);
    void setTime(float time);
    void setZoom(float value);
    void setFont(Font font);

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
    void flush3D();
    void flush2D();
    //--------------------------------------------------- draw -------------------------------------------------------//
    void drawText(const std::string& text,float x,float y,const std::string& align);
    void drawCircle(float x, float y, float r, bool fill);
    void drawSprite(TextureRegion& texture_region, float x, float y, float angle, float scale_x, float scale_y);
    void drawRectangle(float x, float y, float w, float h, bool fill);
    void drawMesh(Mesh* mesh, const glm::mat4& transform);
    void drawScene(Model* model, const glm::mat4& transform);
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
