#include "Block.hpp"
#include "Common.hpp"

#include <lua.hpp>

//Block
Block::Block() {

}

vec2i Block::getTextureCoord(EnumDirection dir) {
    return vec2i(0, 0);
}

bool Block::isSolid() {
    return true;
}

//LuaBlock
LuaBlock::LuaBlock(int blockID) {
    this->blockID = blockID;

    lua_pushstring(Common::lua, "BLOCK_REGISTRY");
    lua_gettable(Common::lua, LUA_REGISTRYINDEX);
    lua_pushinteger(Common::lua, blockID);
    lua_gettable(Common::lua, -2);
    lua_pushstring(Common::lua, "name");
    lua_gettable(Common::lua, -2);
    this->name = std::string(lua_tostring(Common::lua, -1));
}

vec2i LuaBlock::getTextureCoord(EnumDirection dir) {
    lua_pushstring(Common::lua, "BLOCK_REGISTRY");
    lua_gettable(Common::lua, LUA_REGISTRYINDEX);
    lua_pushinteger(Common::lua, blockID);
    lua_gettable(Common::lua, -2);
    lua_pushstring(Common::lua, "getTextureCoord");
    lua_gettable(Common::lua, -2);
    lua_pushinteger(Common::lua, static_cast<int>(dir));
    lua_call(Common::lua, 1, 2);

    vec2i coord = vec2i(lua_tointeger(Common::lua, -2), lua_tointeger(Common::lua, -1));
    lua_settop(Common::lua, 1);

    return coord;
}

bool LuaBlock::isSolid() {
    lua_pushstring(Common::lua, "BLOCK_REGISTRY");
    lua_gettable(Common::lua, LUA_REGISTRYINDEX);
    lua_pushinteger(Common::lua, blockID);
    lua_gettable(Common::lua, -2);
    lua_pushstring(Common::lua, "isSolid");
    lua_gettable(Common::lua, -2);
    lua_call(Common::lua, 0, 1);

    bool solid =  lua_toboolean(Common::lua, -1);
    lua_settop(Common::lua, 1);

    return solid;
}

//BlockRegistry
std::map<int, Block*> BlockRegistry::registry;

void BlockRegistry::registerBlock(int id, Block *block) {
    registry.insert(std::make_pair(id, block));
}

Block *BlockRegistry::getBlock(int id) {
    return registry[id];
}

int BlockRegistry::registeredBlocks() {
    return registry.size();
}
