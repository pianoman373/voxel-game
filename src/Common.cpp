#include "Common.hpp"
#include "Block.hpp"
#include "World.hpp"

#include <lua.hpp>

World Common::world;

lua_State *Common::lua;

static int registerBlock(lua_State *L) {
    if (lua_isnumber(L, 1) && lua_istable(L, 2)) {
        lua_pushstring(L, "BLOCK_REGISTRY");
        lua_gettable(L, LUA_REGISTRYINDEX);
        lua_pushvalue(L, 1);
        lua_pushvalue(L, 2);
        lua_settable(L, -3);

        int ID = lua_tointeger(L, 1);
        BlockRegistry::registerBlock(ID, new LuaBlock(ID));
    }

    return 0;
}

void Common::init() {
    int status, result;

    lua = lua_open();
    luaL_openlibs(lua);
    lua_pushcfunction(lua, registerBlock);
    lua_setglobal(lua, "registerBlock");

    lua_pushstring(lua, "BLOCK_REGISTRY");
    lua_newtable(lua);
    lua_settable(lua, LUA_REGISTRYINDEX);

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
