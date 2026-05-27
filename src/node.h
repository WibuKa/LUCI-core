#pragma once
#include <glm/glm.hpp>
#include <string>

class Node {
public:
    bool visible = true;
    unsigned int id = 0;
    std::string name = "";

    glm::mat4 localTransform = glm::mat4(1.0f);
    glm::mat4 worldTransform = glm::mat4(1.0f);

    Node* parent = nullptr;
    std::vector<Node*> children;
    
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
