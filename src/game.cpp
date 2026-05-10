#include "game.h"
#include "lua_API.h"
#include "renderer.h"

namespace Game{
    unsigned int id;

float runTime = 0.0;

void load()
{
    Lua::create();
    Lua::load();
}

void update(float deltaTime)
{
    runTime += deltaTime;
    Lua::update(deltaTime);
    Lua::draw();
    Render::setTime(runTime);
}

}
