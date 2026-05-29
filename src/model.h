#pragma once
#include <tiny_gltf.h>
#include "light.h"
#include "mesh.h"
#include "node.h"
#include "bone.h"

class Model {
private:
    tinygltf::Model model;
    std::vector<unsigned int> skinBoneOffsets;
   
    Camera* buildCamera(unsigned int id);
    Light* buildLight(unsigned int id);
    Mesh* buildMesh(unsigned int id);
    Node* buildNode(int nodeIndex);
  
    void buildCameras();
    void buildLights();
    void buildMeshes();
    void buildNodes();
    void buildBones();

    void buildTreeString(Node* node, std::string& out, const std::string& prefix, bool isLast, bool isRoot = false);
    void buildRootNode();
public:
    std::vector<Node*> nodes;
    std::vector<Bone*> bones;
    std::vector<Mesh*> meshes;
    std::vector<Light*> lights;
    std::vector<Camera*> cameras;
    
    Node* rootNode;
    
    Model(tinygltf::Model gltfModel);
    ~Model();

    unsigned int getNodeCount();
    unsigned int getMeshCount();
    unsigned int getBoneCount();

    Mesh*   getMesh(unsigned int id);
    Node*   getNode(unsigned int id);
    Bone*   getBone(unsigned int id);
    Light*  getLight(unsigned int id);
    Camera* getCamera(unsigned int id);

    void printRootNode();
};
