#include "model.h"
#include "delog.h"

Model::Model(tinygltf::Model gltfModel) : model(std::move(gltfModel))
{
    rootNode = new Node();
    buildRootNode();
    buildBones();
    buildMeshes();
}

Model::~Model()
{
    for (Node* node : nodes)
        delete node;
}
unsigned int Model::getNodeCount(){
    return nodes.size();
}
unsigned int Model::getMeshCount(){
   return meshes.size();
}
unsigned int Model::getBoneCount(){
    return bones.size();
}

Node* Model::getNode(unsigned int id)
{
    return nodes[id];
}

Mesh* Model::getMesh(unsigned int id)
{
    return meshes[id];
}

Bone* Model::getBone(unsigned int id)
{
    return bones[id];
}

Node* Model::buildNode(int nodeIndex)
{
    Node* node = new Node();
    
    node->name = model.nodes[nodeIndex].name;
    node->id   = nodeIndex;
    for (int childIndex : model.nodes[nodeIndex].children){
        Node* child = buildNode(childIndex);
        child->parent = node;
        node->children.push_back(child);
    }

    nodes[nodeIndex] = node;
    return node;
}

void Model::buildRootNode()
{
    rootNode->id = UINT_MAX;
    rootNode->name = "Root";
    
    nodes.resize(model.nodes.size(),nullptr);
    for (size_t i = 0; i < model.nodes.size(); i++)
    {
        if (nodes[i] == nullptr) buildNode(i);
    }

    for (int nodeIndex : model.scenes[0].nodes)
    {
        rootNode->addChild(nodes[nodeIndex]);
    }
}

void Model::buildBones()
{
    int boneIDCount = 0;
    int skinIDCount = 0;
    
    size_t boneSize = 0;
    for (const tinygltf::Skin& skin : model.skins) boneSize += skin.joints.size();
    bones.resize(boneSize);

    for (const tinygltf::Skin& skin : model.skins)
    {
        skinBoneOffsets.push_back(boneIDCount);
        for (int jointNodeIndex : skin.joints)
        {
            Bone* bone = new Bone();

            bone->id             = boneIDCount;
            bone->node           = getNode(jointNodeIndex);
            bone->name           = bone->node->name;
            bone->skinBoneOffset = skinIDCount;

            bones[boneIDCount] = bone;
            boneIDCount++;
        }
        skinIDCount++;
    }
}

void Model::buildTreeString(Node* node, std::string& out, const std::string& prefix, bool isLast, bool isRoot)
{
    out += prefix;
    out += (isRoot ? "" :(isLast ? "└── " : "├── "));
    out += node->name + (isRoot ? "" : " ["+std::to_string(node->id)+"]");
    out += "\n";

    for (size_t i = 0; i < node->children.size(); i++)
    {
        bool childLast = (i == node->children.size() - 1);

        buildTreeString(
            node->children[i],
            out,
            prefix + (isRoot ? "" : "    "),
            childLast
        );
    }
}

void Model::printRootNode()
{
    std::string out;
    buildTreeString(rootNode, out, "", true, true);
    Delog::msg("%s", out.c_str());
}

Mesh* Model::buildMesh(unsigned int id)
{
    const tinygltf::Mesh& gltfMesh = model.meshes[id];
   
    // Find Skin Offset Index
    int skinOffsetIndex = -1;
    for (const tinygltf::Node& node : model.nodes)
    {
        if (node.mesh == id)
        {
            skinOffsetIndex = node.skin;
            break;
        }
    }

    Mesh* mesh = new Mesh();
    mesh->name = gltfMesh.name;
    
    for (const tinygltf::Primitive& gltfPrimitive : gltfMesh.primitives)
    {
        Primitive primitive;

        // POSITION //
        auto posIt = gltfPrimitive.attributes.find("POSITION");

        if (posIt == gltfPrimitive.attributes.end())
            continue;

        const tinygltf::Accessor& posAccessor = model.accessors[posIt->second];
        const tinygltf::BufferView& posView   = model.bufferViews[posAccessor.bufferView];
        const tinygltf::Buffer& posBuffer     = model.buffers[posView.buffer];

        const float* positions =
            reinterpret_cast<const float*>(
                posBuffer.data.data() +
                posView.byteOffset +
                posAccessor.byteOffset
            );

        // NORMAL //
        const float* normals = nullptr;

        auto normalIt = gltfPrimitive.attributes.find("NORMAL");

        if (normalIt !=
            gltfPrimitive.attributes.end())
        {
            const tinygltf::Accessor& accessor = model.accessors[normalIt->second];
            const tinygltf::BufferView& view   = model.bufferViews[accessor.bufferView];
            const tinygltf::Buffer& buffer     = model.buffers[view.buffer];

            normals = reinterpret_cast<const float*>(
                    buffer.data.data() +
                    view.byteOffset +
                    accessor.byteOffset
                );
        }

        // TEXCOORD //
        const float* uvs = nullptr;

        auto uvIt = gltfPrimitive.attributes.find("TEXCOORD_0");

        if (uvIt != gltfPrimitive.attributes.end())
        {
            const tinygltf::Accessor& accessor = model.accessors[uvIt->second];
            const tinygltf::BufferView& view   = model.bufferViews[accessor.bufferView];
            const tinygltf::Buffer& buffer     = model.buffers[view.buffer];

            uvs = reinterpret_cast<const float*>(
                    buffer.data.data() +
                    view.byteOffset +
                    accessor.byteOffset
                );
        }

        // JOINTS_0 //
        const unsigned char* jointsData = nullptr;

        int jointsComponentType = 0;

        auto jointsIt = gltfPrimitive.attributes.find("JOINTS_0");

        if (jointsIt != gltfPrimitive.attributes.end())
        {
            const tinygltf::Accessor& accessor = model.accessors[jointsIt->second];
            const tinygltf::BufferView& view   = model.bufferViews[accessor.bufferView];
            const tinygltf::Buffer& buffer     = model.buffers[view.buffer];

            jointsData = buffer.data.data() + view.byteOffset + accessor.byteOffset;

            jointsComponentType = accessor.componentType;
        }

        // WEIGHTS_0 //
        const float* weights = nullptr;

        auto weightsIt = gltfPrimitive.attributes.find("WEIGHTS_0");

        if (weightsIt != gltfPrimitive.attributes.end())
        {
            const tinygltf::Accessor& accessor = model.accessors[weightsIt->second];
            const tinygltf::BufferView& view   = model.bufferViews[accessor.bufferView];
            const tinygltf::Buffer& buffer     = model.buffers[view.buffer];

            weights = reinterpret_cast<const float*>(
                    buffer.data.data() +
                    view.byteOffset +
                    accessor.byteOffset);
        }

        // BUILD VERTICES //
        for (size_t i = 0; i < posAccessor.count; i++)
        {
            Vertex3D vertex;

            // POSITION
            vertex.position = glm::vec3(
                positions[i * 3 + 0],
                positions[i * 3 + 1],
                positions[i * 3 + 2]
            );

            // NORMAL
            if (normals)
            {
                vertex.normal = glm::vec3(
                    normals[i * 3 + 0],
                    normals[i * 3 + 1],
                    normals[i * 3 + 2]
                );
            }

            // UV
            if (uvs)
            {
                vertex.texCoord = glm::vec2(
                    uvs[i * 2 + 0],
                    uvs[i * 2 + 1]
                );
            }

            // BONE IDS + WEIGHTS
            
            if (jointsData && weights)
            {
                unsigned int boneOffset = 0;
                if (skinOffsetIndex >= 0 && (size_t)skinOffsetIndex < skinBoneOffsets.size())
                    boneOffset = skinBoneOffsets[skinOffsetIndex];

                if (jointsComponentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
                {
                    const uint8_t* joints = reinterpret_cast<const uint8_t*>(jointsData);

                    vertex.boneIDs = glm::ivec4(
                            joints[i * 4 + 0] + boneOffset,
                            joints[i * 4 + 1] + boneOffset,
                            joints[i * 4 + 2] + boneOffset,
                            joints[i * 4 + 3] + boneOffset
                        );
                }
                else if (jointsComponentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
                {
                    const uint16_t* joints = reinterpret_cast<
                            const uint16_t*>(
                                jointsData
                            );

                    vertex.boneIDs = glm::ivec4(
                            joints[i * 4 + 0] + boneOffset,
                            joints[i * 4 + 1] + boneOffset,
                            joints[i * 4 + 2] + boneOffset,
                            joints[i * 4 + 3] + boneOffset
                        );
                }

                vertex.weights = glm::vec4(
                        weights[i * 4 + 0],
                        weights[i * 4 + 1],
                        weights[i * 4 + 2],
                        weights[i * 4 + 3]
                    );
            }

            primitive.vertices.push_back(vertex);
        }

        // INDICES //
        if (gltfPrimitive.indices >= 0)
        {
            const tinygltf::Accessor&
                indexAccessor = model.accessors[gltfPrimitive.indices];

            const tinygltf::BufferView&
                indexView = model.bufferViews[indexAccessor.bufferView];

            const tinygltf::Buffer&
                indexBuffer = model.buffers[indexView.buffer];

            const unsigned char* data =
                indexBuffer.data.data() +
                indexView.byteOffset +
                indexAccessor.byteOffset;

            for (size_t i = 0; i < indexAccessor.count; i++){
                unsigned int index = 0;

                switch (indexAccessor.componentType)
                {
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                        index = reinterpret_cast<const uint8_t*>(data)[i];
                        break;

                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                        index =reinterpret_cast<const uint16_t*>(data)[i];
                        break;

                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                        index = reinterpret_cast<const uint32_t*>(data)[i];
                        break;
                }
                primitive.indices.push_back(
                    index
                );
            }
        }

        // MATERIAL //
        if (gltfPrimitive.material >= 0)
        {
            primitive.material.name = model.materials[gltfPrimitive.material].name;
        }

        mesh->primitives.push_back(std::move(primitive));
    }

    return mesh;
}

void Model::buildMeshes()
{
    meshes.resize(model.meshes.size());
    for (size_t i = 0; i < model.meshes.size(); i++) meshes[i] = buildMesh(i);
}
