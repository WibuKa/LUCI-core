#include "game.h"
#include "loader.h"
#include "lua_API.h"
#include "renderer.h"
#include "model.h"
#include "texture_region.h"
#include "delog.h"
#include <vector>

namespace Game{

unsigned int id;
float runTime = 0.0;
Mesh* mesh;
Texture texture; 
TextureRegion texture_region;

std::vector<glm::vec2> points;

void init()
{
    Lua::create();
    Lua::load();

    std::srand(std::time(nullptr));

    Model model = Loader::loadModel("cube.gltf");
    texture = Loader::loadTexture("image.png");
    texture_region = TextureRegion(texture, 0, 0, texture.getWidth(), texture.getHeight());
        
    points.reserve(100000);
    for (int i = 0; i < 100000; i++)
    {
        points.push_back(glm::vec2(std::rand() % 1920, std::rand() % 1080));
    }

    printf("Object count: %d\n", model.getNodeCount());
    model.printRootNode();
    mesh = model.getMesh(0);

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
    printf("fps: %f\n", 1.0f /deltaTime);
    Lua::update(deltaTime);

}

void draw()
{    
    Render::setTime(runTime);
    Render::beginBatch();
    for (glm::vec2 &point : points)
    {
        Render::drawSprite(texture_region, point.x, point.y, 0, 1, 1);
    }
    //Render::drawMesh(mesh);
    Lua::draw();
    Render::endBatch();
}

}
