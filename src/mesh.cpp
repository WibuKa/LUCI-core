#include "mesh.h"

void Mesh::addPrimitive(Primitive& primitive)
{
    primitives.push_back(primitive);
}

void Mesh::createBuffer(const std::vector<Vertex3D>& vertices, const std::vector<unsigned int>& indices)
{
    if (vertices.empty() || indices.empty()) return;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex3D), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Position (vec3) - Location 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, position));
    glEnableVertexAttribArray(0);

    // TexCoord (vec2) - Location 1
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, texCoord));
    glEnableVertexAttribArray(1);

    // Normal (vec3) - Location 2
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, normal));
    glEnableVertexAttribArray(2);

    // BoneIDs (ivec4) - Location 4
    glVertexAttribIPointer(4, 4, GL_INT, sizeof(Vertex3D), (void*)offsetof(Vertex3D, boneIDs));
    glEnableVertexAttribArray(4);

    // Weights (vec4) - Location 5
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, weights));
    glEnableVertexAttribArray(5);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
