#include "Common.hpp"
#include "Block.hpp"
#include "World.hpp"

#include "sol.hpp"

World Common::world;

static void registerBlockNew(int id, sol::table block) {
    std::cout << "registering block id " << id << std::endl;

    BlockRegistry::registerBlock(id, new LuaBlock(block));
}

bool alreadyInitted = false;

struct test {
    int i;
    int j;
};

sol::state luaState;

void Common::init() {
    if (alreadyInitted)
        return;

    alreadyInitted = true;

    // open some common libraries
    luaState.open_libraries(sol::lib::base, sol::lib::package);
    luaState["registerBlock"] = registerBlockNew;
    luaState.script_file("api.lua", &sol::default_on_error);
    luaState.script_file("init.lua", &sol::default_on_error);
}
