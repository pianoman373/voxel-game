#pragma once

#include "sol.hpp"

class LuaHandler {
public:
    sol::state state;

    LuaHandler();

    void init();

    void runScripts();
};