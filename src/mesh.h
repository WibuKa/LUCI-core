#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "primitive.h"
#include "vertex.h"

class Mesh {
    
public:
    std::string name;

    std::vector<Primitive> primitives;
    
    unsigned int VAO;
    unsigned int EBO;
    unsigned int VBO;
    
    void addPrimitive(Primitive& primitive);
    void createBuffer(const std::vector<Vertex3D>& vertices, const std::vector<unsigned int>& indices);
};
