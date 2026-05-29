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
Model* model;
Model* model2;
Mesh* mesh1;
Mesh* mesh2;
Mesh* mesh3;
Texture texture; 
TextureRegion texture_region;

std::vector<glm::vec2> points;

float monkey_rot_y = 0.0f;

void init()
{
    Lua::create();
    Lua::load();
    
    // ------------------------------------------------------------------------------------- //
    std::srand(std::time(nullptr));

    printf("Loading dragon.gltf\n");
    model = Loader::loadModel("dragon.gltf");
    model2 = Loader::loadModel("monkey.gltf");
    printf("Loading dragon.gltf done\n");
    texture = Loader::loadTexture("image.png");
    texture_region = TextureRegion(texture, 0, 0, texture.getWidth(), texture.getHeight());
        
    points.reserve(150000);

    printf("Object count: %d\n", model->getNodeCount());
    model->printRootNode();

    mesh1 = model->getMesh(0);
    mesh2 = model->getMesh(1);
    mesh3 = model->getMesh(2);

    for (Bone* bone : model->bones)
    {
        printf("Node: %s", bone->node->name.c_str());
        printf("[%d]\n", bone->id);
    }

    Render::view.rotation = glm::vec3(180.0f, 0.0f, 0.0f);
    Render::view.position = glm::vec3(0.0f, 0.0f, 10.0f);
}

void update(float deltaTime)
{
    runTime += deltaTime;
    //spin 
    monkey_rot_y += deltaTime * 25;
    if (monkey_rot_y > 360.0f) monkey_rot_y = 0.0f;
    //printf("fps: %f\n", 1.0f /deltaTime);
    Lua::update(deltaTime);

}

void draw()
{
    //spin

    Render::setTime(runTime);
    Render::beginBatch();
    glm::mat4 mesh1_transform = glm::mat4(1.0f);
    mesh1_transform = glm::rotate(mesh1_transform, glm::radians(monkey_rot_y), glm::vec3(0.0f, 1.0f, 0.0f));
    //Render::drawMesh(mesh1,mesh1_transform);
    //Render::drawMesh(mesh2,glm::mat4(2.0f));
    //Render::drawMesh(mesh3,glm::mat4(3.0f));
    Render::drawScene(model,mesh1_transform);
    Lua::draw();
    Render::endBatch();
}

}
