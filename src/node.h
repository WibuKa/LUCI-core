#pragma once
#include <glm/glm.hpp>
#include <string>
#include "mesh.h"
#include "light.h"
#include "camera.h"

class Node {
    enum Type {
        NODE,
        MESH,
        CAMERA,
        LIGHT,
    };
public:
    bool visible = true;
    unsigned int id = 0;
    std::string name = "";

    glm::mat4 transform       = glm::mat4(1.0f);
    glm::mat4 globalTransform = glm::mat4(1.0f);

    Node* parent = nullptr;
    std::vector<Node*> children;

    Mesh* mesh     = nullptr;
    Light* light   = nullptr;
    Camera* camera = nullptr;

    // ------------------------//
    void setParent(Node* parent){
        this->parent = parent;
    }
    void addChild(Node* child){
        printf("addChild: %s", child->name.c_str());
        child->setParent(this);
        children.push_back(child);
    }
};
