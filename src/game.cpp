#include "game.h"
#include "luaAPI.h"

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
