#include "Block.hpp"
#include "Common.hpp"

static Block defaultBlock;

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
LuaBlock::LuaBlock(int id) {
    this->id = id;

    sol::table table = Common::luaState["__blocks"][id];
    this->name = table["name"];
    this->color = vec3(table["color"][1], table["color"][2], table["color"][3]);

    sol::safe_function fun = table.get<sol::safe_function>("isSolid");
    solid = fun();
}

vec2i LuaBlock::getTextureCoord(EnumDirection dir) {
    sol::table table = Common::luaState["__blocks"][id];
    sol::safe_function f = table.get<sol::safe_function>("getTextureCoord");
    std::tuple<int, int> coord = f(static_cast<int>(dir));

    return vec2i(std::get<0>(coord), std::get<1>(coord));
    //return vec2i();
}

bool LuaBlock::isSolid() {
    return solid;
}

//BlockRegistry
std::map<int, Block*> BlockRegistry::registry;

void BlockRegistry::registerBlock(int id, Block *block) {
    registry.insert(std::make_pair(id, block));
}

Block *BlockRegistry::getBlock(int id) {
    //try {
        return registry.at(id);
//    }
//    catch (std::out_of_range exception) {
//        return &defaultBlock;
//    }
}

int BlockRegistry::registeredBlocks() {
    return registry.size();
}
