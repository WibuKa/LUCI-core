#pragma once
#include <glad/glad.h>
#include <vector>
#include "material.h"
#include "vertex.h"

class Primitive {
    public:
    
    Primitive() = default;
    ~Primitive();

    Primitive(const Primitive&) = delete;
    Primitive& operator=(const Primitive&) = delete;

    Primitive(Primitive&& other) noexcept
        : vertices(std::move(other.vertices))
        , indices(std::move(other.indices))
        , VAO(other.VAO), VBO(other.VBO), EBO(other.EBO)
        , material(std::move(other.material))
    {
        other.VAO = 0;
        other.VBO = 0;
        other.EBO = 0;
    }

    Primitive& operator=(Primitive&& other) noexcept
    {
        if (this != &other)
        {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);

            vertices = std::move(other.vertices);
            indices  = std::move(other.indices);
            VAO = other.VAO; VBO = other.VBO; EBO = other.EBO;
            material = std::move(other.material);

            other.VAO = 0;
            other.VBO = 0;
            other.EBO = 0;
        }
        return *this;
    }

    std::vector<Vertex3D> vertices;
    std::vector<unsigned int> indices;
    
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;

    Material material;

    void createBuffer();
};
