#pragma once
#include <glm/glm.hpp>
#include "color.h"

#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

struct Light {
    int type = DIRECTIONAL_LIGHT;

    // Common
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

    float intensity = 1.0f;
    float distance  = 0.0f;

    // Directional Light
    glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);

    // Spot Light
    float spotAngle = glm::radians(45.0f);
    float spotExponent = 0.0f;
};
