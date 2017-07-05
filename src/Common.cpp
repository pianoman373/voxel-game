#include "Common.hpp"
#include "Block.hpp"

#include <lua.hpp>

World Common::world;

lua_State *Common::lua;

static void stackDump (lua_State *L) {
  int i;
  int top = lua_gettop(L);
  for (i = 1; i <= top; i++) {  /* repeat for each level */
    int t = lua_type(L, i);
    switch (t) {

      case LUA_TSTRING:  /* strings */
        printf("`%s'", lua_tostring(L, i));
        break;

      case LUA_TBOOLEAN:  /* booleans */
        printf(lua_toboolean(L, i) ? "true" : "false");
        break;

      case LUA_TNUMBER:  /* numbers */
        printf("%g", lua_tonumber(L, i));
        break;

      default:  /* other values */
        printf("%s", lua_typename(L, t));
        break;

    }
    printf("  ");  /* put a separator */
  }
  printf("\n");  /* end the listing */
}

static int registerBlock(lua_State *L) {
    if (lua_isnumber(L, 1) && lua_istable(L, 2)) {
        lua_pushstring(L, "BLOCK_REGISTRY");
        lua_gettable(L, LUA_REGISTRYINDEX);
        lua_pushvalue(L, 1);
        lua_pushvalue(L, 2);
        //stackDump(lua);
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

void Common::update(float delta) {

}
