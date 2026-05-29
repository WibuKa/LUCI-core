#include "scene.h"
#include "camera.h"
#include "delog.h"
#include "light.h"
#include "material.h"
#include <glm/gtc/type_ptr.hpp>

Scene::Scene(tinygltf::Model gltfModel) : model(std::move(gltfModel))
{
    rootNode = new Node();
    buildLights();
    buildMeshes();
    buildNodes();
    buildRootNode();
    buildBones();
}

Scene::~Scene()
{
    for (Node* node : nodes)
        delete node;
}
unsigned int Scene::getNodeCount(){
    return nodes.size();
}
unsigned int Scene::getMeshCount(){
   return meshes.size();
}
unsigned int Scene::getBoneCount(){
    return bones.size();
}

Node* Scene::getNode(unsigned int id)
{
    return nodes[id];
}

Mesh* Scene::getMesh(unsigned int id)
{
    return meshes[id];
}

Bone* Scene::getBone(unsigned int id)
{
    return bones[id];
}

Light* Scene::getLight(unsigned int id)
{
    return lights[id];
}

Camera* Scene::getCamera(unsigned int id)
{
    return nullptr;
}

Node* Scene::buildNode(int nodeIndex)
{
    const tinygltf::Node& gltfNode = model.nodes[nodeIndex];
    Node* node = new Node();
    
    node->name = model.nodes[nodeIndex].name;
    node->id   = nodeIndex;
    
    if (gltfNode.matrix.size() == 16)
    {
        node->globalTransform = glm::make_mat4x4(gltfNode.matrix.data());
    }
    else
    {
        glm::mat4 tra = glm::mat4(1.0f);
        glm::mat4 rot = glm::mat4(1.0f);
        glm::mat4 sca = glm::mat4(1.0f);
        
        // Translation
        if (gltfNode.translation.size() == 3) {
            tra = glm::translate(glm::mat4(1.0f), glm::vec3(gltfNode.translation[0], gltfNode.translation[1], gltfNode.translation[2]));
        }
        // Rotation 
        if (gltfNode.rotation.size() == 4) {
            glm::quat q = glm::quat(gltfNode.rotation[3], gltfNode.rotation[0], gltfNode.rotation[1], gltfNode.rotation[2]);
            rot = glm::mat4_cast(q);
        }
        // Scale 
        if (gltfNode.scale.size() == 3) {
            sca = glm::scale(glm::mat4(1.0f), glm::vec3(gltfNode.scale[0], gltfNode.scale[1], gltfNode.scale[2]));
        }

        node->globalTransform = tra * rot * sca;
    }

    if (model.nodes[nodeIndex].mesh >= 0){
        node->mesh = getMesh(model.nodes[nodeIndex].mesh);
    }

    if (model.nodes[nodeIndex].camera >= 0)
        node->camera = getCamera(model.nodes[nodeIndex].camera);

    if (model.nodes[nodeIndex].light >= 0)
        node->light = getLight(model.nodes[nodeIndex].light);
   

    for (int childIndex : model.nodes[nodeIndex].children){
        Node* child = buildNode(childIndex);
        child->parent = node;
        node->children.push_back(child);
    }

    nodes[nodeIndex] = node;
    return node;
}

void Scene::buildNodes()
{
    nodes.resize(model.nodes.size(),nullptr);
    for (size_t i = 0; i < model.nodes.size(); i++)
    {
        if (nodes[i] == nullptr) buildNode(i);
    }
}

void Scene::buildRootNode()
{
    rootNode->id = UINT_MAX;
    rootNode->name = "Root";
    
    for (int nodeIndex : model.scenes[0].nodes)
    {
        rootNode->addChild(nodes[nodeIndex]);
    }
}

void Scene::buildBones()
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

void Scene::buildTreeString(Node* node, std::string& out, const std::string& prefix, bool isLast, bool isRoot)
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

void Scene::printRootNode()
{
    std::string out;
    buildTreeString(rootNode, out, "", true, true);
    Delog::msg("%s", out.c_str());
}

Camera* Scene::buildCamera(unsigned int id)
{
    Camera* camera = new Camera();
    tinygltf::Camera& gltfCamera = model.cameras[id];

    if (gltfCamera.type == "perspective")
    {
        camera->type = PERSPECTIVE_CAMERA;
        camera->fov  = gltfCamera.perspective.yfov;
        camera->near = gltfCamera.perspective.znear;
        camera->far  = gltfCamera.perspective.zfar;
    }
    else if (gltfCamera.type == "orthographic")
    {
        camera->type = ORTHOGRAPHIC_CAMERA;
        camera->near = gltfCamera.orthographic.znear;
        camera->far  = gltfCamera.orthographic.zfar;
    }

    return camera;
}

void Scene::buildCameras()
{
    cameras.resize(model.cameras.size());
    for (size_t i = 0; i < model.cameras.size(); i++) cameras[i] = buildCamera(i);
}

Mesh* Scene::buildMesh(unsigned int id)
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

    std::vector<Vertex3D> verticesBuffer;
    std::vector<unsigned int> indicesBuffer;
    
    unsigned int vertexOffset  = 0;
    unsigned int indicesOffset = 0;

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
            
            verticesBuffer.push_back(vertex);
        }

        const tinygltf::Accessor&indexAccessor = model.accessors[gltfPrimitive.indices];
        const tinygltf::BufferView& indexView  = model.bufferViews[indexAccessor.bufferView];
        const tinygltf::Buffer& indexBuffer    = model.buffers[indexView.buffer];

        // INDICES //
        if (gltfPrimitive.indices >= 0)
        {
            const unsigned char* data = indexBuffer.data.data() + indexView.byteOffset + indexAccessor.byteOffset;

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
                indicesBuffer.push_back(index);
            }
        }

        // MATERIAL //
        if (gltfPrimitive.material >= 0)
        {
            // MATERIAL NAME
            primitive.material.name = model.materials[gltfPrimitive.material].name;
            const tinygltf::Material& gltfMaterial = model.materials[gltfPrimitive.material];

            // BASE COLOR
            if (gltfMaterial.pbrMetallicRoughness.baseColorTexture.index >= 0) {
                Texture baseColorTexture = Texture();
                int texIndex = gltfMaterial.pbrMetallicRoughness.baseColorTexture.index;
                const tinygltf::Image& img = model.images[model.textures[texIndex].source];
                const unsigned char* pixelData = img.image.data();

                baseColorTexture.name = model.textures[texIndex].name;
                baseColorTexture.create(pixelData, img.width, img.height, img.component);
               
                primitive.material.textures[BASE_COLOR_TEXTURE_SLOT] = baseColorTexture;
                printf("----------------------------\n");
                printf("baseColorTexture\n");
            }

            // NORMAL MAP
            if (gltfMaterial.normalTexture.index >= 0)
            {
                Texture normalTexture = Texture();
                int texIndex = gltfMaterial.normalTexture.index;
                const tinygltf::Image& img = model.images[model.textures[texIndex].source];
                const unsigned char* pixelData = img.image.data();

                normalTexture.name = model.textures[texIndex].name;
                normalTexture.create(pixelData, img.width, img.height, img.component);

                primitive.material.textures[NORMAL_TEXTURE_SLOT] = normalTexture;
                printf("normalTexture\n");
            }

            // METALLIC ROUGHNESS
            if (gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index >= 0)
            {
                Texture metallicRoughnessTexture = Texture();
                int texIndex = gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index;
                const tinygltf::Image& img = model.images[model.textures[texIndex].source];
                const unsigned char* pixelData = img.image.data();

                metallicRoughnessTexture.name = model.textures[texIndex].name;
                metallicRoughnessTexture.create(pixelData, img.width, img.height, img.component);

                primitive.material.textures[METALLIC_TEXTURE_SLOT] = metallicRoughnessTexture;
                printf("metallicRoughnessTexture\n");
            }
            
            // OCCLUSION
            if (gltfMaterial.occlusionTexture.index >= 0)
            {
                Texture occlusionTexture = Texture();
                int texIndex = gltfMaterial.occlusionTexture.index;
                const tinygltf::Image& img = model.images[model.textures[texIndex].source];
                const unsigned char* pixelData = img.image.data();
                occlusionTexture.name = model.textures[texIndex].name;
                occlusionTexture.create(pixelData, img.width, img.height, img.component);
                primitive.material.textures[OCCLUSION_TEXTURE_SLOT] = occlusionTexture;
            }

            if (gltfMaterial.emissiveTexture.index >= 0)
            {
                Texture emissiveTexture = Texture();
                int texIndex = gltfMaterial.emissiveTexture.index;
                const tinygltf::Image& img = model.images[model.textures[texIndex].source];
                const unsigned char* pixelData = img.image.data();

                emissiveTexture.name = model.textures[texIndex].name;
                emissiveTexture.create(pixelData, img.width, img.height, img.component);

                primitive.material.textures[EMISSIVE_TEXTURE_SLOT] = emissiveTexture;
            }
        }
        primitive.vertexOffset  = vertexOffset;
        primitive.vertexCount   = posAccessor.count;
        primitive.indicesOffset = indicesOffset;
        primitive.indicesCount  = indexAccessor.count;

        vertexOffset  += posAccessor.count;
        indicesOffset += indexAccessor.count;
        
        mesh->addPrimitive(primitive);
    }
    
    mesh->createBuffer(verticesBuffer, indicesBuffer);
    return mesh;
}

void Scene::buildMeshes()
{
    meshes.resize(model.meshes.size());
    for (size_t i = 0; i < model.meshes.size(); i++) meshes[i] = buildMesh(i);
}

Light* Scene::buildLight(unsigned int id)
{
    const tinygltf::Light& gltfLight = model.lights[id];
    Light* light = new Light();

    light->intensity = gltfLight.intensity;
    light->color = glm::vec3(gltfLight.color[0], gltfLight.color[1], gltfLight.color[2]);
    

    if (gltfLight.type == "directional")
        light->type = DIRECTIONAL_LIGHT;
    else if (gltfLight.type == "point")
        light->type = POINT_LIGHT;
    else if (gltfLight.type == "spot")
        light->type = SPOT_LIGHT;

    if (gltfLight.range <= 0.0f)
        light->distance = std::sqrt(light->intensity / 0.01f);
    else
        light->distance = gltfLight.range;

    return light;
}

void Scene::buildLights()
{
    lights.resize(model.lights.size());
    for (size_t i = 0; i < model.lights.size(); i++) lights[i] = buildLight(i);
}
