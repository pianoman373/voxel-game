#include "Block.hpp"
#include "Client.hpp"

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

bool isSolid();

//LuaBlock
LuaBlock::LuaBlock(sol::table table) {
    name = table["name"].get_or(std::string("Untitled"));
    solid = table["solid"].get_or(true);

    sol::table textures = table["textures"];

    if (textures == sol::nil) {
        for (int i = 0; i < 6; i++) {
            textureCoords.push_back({0, 0});
        }
    }
    else if (textures.size() == 1) {
        for (int i = 0; i < 6; i++) {
            textureCoords.push_back({textures[1][1], textures[1][2]});
        }
    }
    else if (textures.size() == 6) {
        for (int i = 0; i < 6; i++) {
            textureCoords.push_back({textures[i+1][1], textures[i+1][2]});
        }
    }
    else {
        for (int i = 0; i < 6; i++) {
            textureCoords.push_back({0, 0});
        }
    }
}

vec2i LuaBlock::getTextureCoord(EnumDirection dir) {
    return textureCoords[static_cast<int>(dir)];
}

bool LuaBlock::isSolid() {
    return solid;
}

//BlockRegistry
std::map<int, Block*> BlockRegistry::registry;

void BlockRegistry::registerBlock(int id, Block *block) {
    registry.insert(std::make_pair(id, block));
}

void BlockRegistry::registerBlockLua(int id, sol::table block) {
    registerBlock(id, new LuaBlock(block));
}

Block *BlockRegistry::getBlock(int id) {
    try {
        return registry.at(id);
    }
    catch (std::out_of_range exception) {
        return &defaultBlock;
    }
}

int BlockRegistry::registeredBlocks() {
    return registry.size();
}