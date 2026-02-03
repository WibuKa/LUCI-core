#include "game.h"
#include "luaPlus.h"
#include "renderer/api.h"

namespace Game{
    unsigned int id;

void load()
{
    Lua::create();
    Lua::load();
}

void update(float deltaTime)
{
    Lua::update(deltaTime);
    Lua::draw();
}

}