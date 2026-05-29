#pragma once
#include <glad/glad.h>
#include "material.h"

struct Primitive {
    unsigned int vertexOffset  = 0;
    unsigned int vertexCount   = 0;
    
    unsigned int indicesOffset = 0;
    unsigned int indicesCount  = 0;

    Material material;
};
