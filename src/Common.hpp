#pragma once

#include "World.hpp"

/**
 * Controller of all tasks/variables that need to be created on both server and client-side.
 */
class Common {
public:
    static World world;

    static void init();

    static void update(float delta);
};
