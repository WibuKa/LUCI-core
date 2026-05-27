#pragma once
#include <vector>
#include "bone.h"
#include "material.h"
#include "vertex.h"

struct Primitive {
    std::vector<Vertex3D> vertices;
    std::vector<unsigned int> indices;
    Material material;
};
