#pragma once
#include <tiny_gltf.h>
#include "mesh.h"
#include "node.h"
#include "bone.h"

class Model {
private:
    tinygltf::Model model;
    std::vector<unsigned int> skinBoneOffsets;
    
    Node* buildNode(int nodeIndex);
    Mesh* buildMesh(unsigned int id);
    
    void buildRootNode();
    void buildTreeString(Node* node, std::string& out, const std::string& prefix, bool isLast, bool isRoot = false);
    void buildBones();
    void buildMeshes();

public:
    std::vector<Node*> nodes;
    std::vector<Bone*> bones;
    std::vector<Mesh*> meshes;
    
    Node* rootNode;
    
    Model(tinygltf::Model gltfModel);
    ~Model();

    unsigned int getNodeCount();
    unsigned int getMeshCount();
    unsigned int getBoneCount();

    Mesh* getMesh(unsigned int id);
    Node* getNode(unsigned int id);
    Bone* getBone(unsigned int id);

    void printRootNode();
};
