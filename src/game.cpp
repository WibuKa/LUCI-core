#include "game.h"
#include "glm/fwd.hpp"
#include "loader.h"
#include "lua_API.h"
#include "renderer.h"
#include "scene.h"
#include "texture_region.h"
#include "delog.h"
#include <vector>

namespace Game{

unsigned int id;
float runTime = 0.0;
Scene* scene;
Scene* scene2;
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
    scene = Loader::loadScene("dragon.gltf");
    scene2 = Loader::loadScene("monkey.gltf");
    printf("Loading dragon.gltf done\n");
    texture = Loader::loadTexture("image.png");
    texture_region = TextureRegion(texture, 0, 0, texture.getWidth(), texture.getHeight());
        
    points.reserve(150000);

    printf("Object count: %d\n", scene->getNodeCount());
    scene->printRootNode();

    mesh1 = scene->getMesh(0);
    mesh2 = scene->getMesh(1);
    mesh3 = scene->getMesh(2);

    for (Bone* bone : scene->bones)
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
    mesh1_transform[3] = glm::vec4(3.0f, 0.0f, 0.0f, 1.0f);
    mesh1_transform = glm::rotate(mesh1_transform, glm::radians(monkey_rot_y), glm::vec3(0.0f, 1.0f, 0.0f));
    //Render::drawMesh(mesh1,mesh1_transform);
    //Render::drawMesh(mesh2,glm::mat4(2.0f));
    //Render::drawMesh(mesh3,glm::mat4(3.0f));
    Render::drawScene(scene,mesh1_transform);
    Render::drawScene(scene2,glm::mat4(2.0f));
    Lua::draw();
    Render::endBatch();
}

}
