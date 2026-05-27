#pragma once
#include <glm/glm.hpp>

struct Vertex2D {
    float x, y;
    float u, v;
    float r, g, b, a;
};

struct Vertex3D {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;

    glm::ivec4 boneIDs = glm::ivec4(0);
    glm::vec4  weights = glm::vec4(0.0f);
};
