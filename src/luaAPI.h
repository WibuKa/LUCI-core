#pragma once
#include <sol/sol.hpp>

namespace Lua{
    extern sol::state lua;
    extern sol::function load;
    extern sol::function update;
    extern sol::function draw;

    void create();
}