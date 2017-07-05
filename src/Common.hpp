#pragma once

#include "World.hpp"
#include <lua.hpp>

/**
 * Controller of all tasks/variables that need to be created on both server and client-side.
 */
class Common {
public:
    static lua_State *lua;

    static World world;

    static void init();

    static void update(float delta);
};
