#include "Common.hpp"
#include "Block.hpp"
#include "World.hpp"

World Common::world;
sol::state Common::luaState;
sol::thread Common::luaThread;

static void registerBlockNew(int id) {
    std::cout << "registering block id " << id << std::endl;

    BlockRegistry::registerBlock(id, new LuaBlock(id));
}

bool alreadyInitted = false;

struct test {
    int i;
    int j;
};

void Common::init() {
    if (alreadyInitted)
        return;

    alreadyInitted = true;

    // open some common libraries
    luaState.open_libraries(sol::lib::base,
                            sol::lib::bit32,
                            sol::lib::coroutine,
                            sol::lib::count,
                            sol::lib::io,
                            sol::lib::math,
                            sol::lib::os,
                            sol::lib::package,
                            sol::lib::string,
                            sol::lib::table,
                            sol::lib::utf8,
                            sol::lib::ffi
                        );
    //luaState.require_script("inspect", luaState.script_file("inspect.lua"));
    luaState["registerBlockNew"] = registerBlockNew;
    luaState.script_file("api.lua", &sol::default_on_error);
    luaState.script_file("init.lua", &sol::default_on_error);

    luaThread = sol::thread::create(luaState);
}
