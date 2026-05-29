#pragma once
#include <glm/glm.hpp>

#define PERSPECTIVE_CAMERA 0
#define ORTHOGRAPHIC_CAMERA 1

class Camera {
public:
    int type = PERSPECTIVE_CAMERA;
    float fov  = 45.0f;
    float near = 0.1f;
    float far  = 100.0f;
};
