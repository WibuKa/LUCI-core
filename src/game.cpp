#include "game.h"
#include "loader.h"
#include "lua_API.h"
#include "renderer.h"
#include "model.h"
#include "texture_region.h"
#include "delog.h"

namespace Game{
    unsigned int id;

float runTime = 0.0;

void init()
{
    Lua::create();
    Lua::load();

    Model model = Loader::loadModel("cube.gltf");
    printf("Object count: %d\n", model.getNodeCount());
    model.printRootNode();
    Mesh* mesh = model.getMesh(0);

    for (Vertex3D& vertex : mesh->primitives[0].vertices)
    {
        printf("%f %f %f\n", vertex.position.x, vertex.position.y, vertex.position.z);
    }

    printf("\n\n\n");

    for (Vertex3D& vertex : mesh->primitives[0].vertices)
    {
        printf("BoneIDs: %d %d %d %d\n", vertex.boneIDs.x, vertex.boneIDs.y, vertex.boneIDs.z, vertex.boneIDs.w);
        printf("Weights: %f %f %f %f\n", vertex.weights.x, vertex.weights.y, vertex.weights.z, vertex.weights.w);
    }

    for (Bone* bone : model.bones)
    {
        printf("Node: %s", bone->node->name.c_str());
        printf("[%d]\n", bone->id);
    }
}

void update(float deltaTime)
{
    runTime += deltaTime;
    Lua::update(deltaTime);

}

void draw()
{    
    Render::setTime(runTime);
    Render::beginBatch();
    Lua::draw();
    Render::endBatch();
}

}
