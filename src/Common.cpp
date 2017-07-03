#include "Common.hpp"
#include "Block.hpp"

#include <lua.hpp>

World Common::world;

void Common::init() {
    BlockRegistry::registerBlock(0, new BlockAir());
    BlockRegistry::registerBlock(1, new BlockGrass());


    world.generate();

    int status, result;

    lua_State *lua = lua_open();
    luaL_openlibs(lua);
    status = luaL_loadfile(lua, "init.lua");
    if (status) {
            /* If something went wrong, error message is at the top of */
            /* the stack */
            printf("Couldn't load file: %s\n", lua_tostring(lua, -1));
            exit(1);
    }

    result = lua_pcall(lua, 0, LUA_MULTRET, 0);
    if (result) {
        printf("Failed to run script: %s\n", lua_tostring(lua, -1));
        exit(1);
    }
}

void Common::update(float delta) {

}
