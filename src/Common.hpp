#pragma once
#include "sol.hpp"
#include <mutex>

class World;

/**
 * Controller of all tasks/variables that need to be created on both server and client-side.
 */
class Common {
public:
    static World world;
    static sol::state luaState;
    static sol::thread luaThread;

    static void init();
};
