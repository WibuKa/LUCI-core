#pragma once
#include <string>
#include <glm/glm.hpp>
#include "node.h"

struct Bone
{
    int id = -1;
    int parent = -1;
    std::string name;
    
    glm::mat4 offsetMatrix;
    glm::mat4 localTransform;
    glm::mat4 globalTransform;
    
    Node* node = nullptr;
    
    int skinBoneOffset = 0;
};
