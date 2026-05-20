#include "game.h"
#include "loader.h"
#include "lua_API.h"
#include "renderer.h"
#include "texture_region.h"

namespace Game{
    unsigned int id;

float runTime = 0.0;

void init()
{
    Lua::create();
    Lua::load();
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
