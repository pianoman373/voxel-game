#pragma once

class lua_State;
class World;
class Lua;

/**
 * Controller of all tasks/variables that need to be created on both server and client-side.
 */
class Common {
public:
    static Lua lua;
    static World world;

    static void init();
};
