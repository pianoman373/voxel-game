#pragma once

class lua_State;
class World;

/**
 * Controller of all tasks/variables that need to be created on both server and client-side.
 */
class Common {
public:
    static lua_State *lua;
    static World world;

    static void init();
};
