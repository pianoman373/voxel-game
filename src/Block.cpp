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
    return false;
}

int Block::getLightLevel() {

}

int Block::getID() {
    return blockID;
}

//LuaBlock
LuaBlock::LuaBlock(sol::table table) {
    name = table["name"].get_or(std::string("Untitled"));
    solid = table["solid"].get_or(true);
    lightLevel = table["lightLevel"].get_or(0);

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

int LuaBlock::getLightLevel() {
    return lightLevel;
}

BlockRegistry::BlockRegistry() {
    for (int i = 0; i < 255; i++) {
        Block *b = new Block();
        b->blockID = i;
        registry.push_back(b);
    }
}

void BlockRegistry::registerBlock(const std::string &id, Block *block) {
    //registry.insert(std::make_pair(id, block));
//
//    block->blockID = id;
//    registry[id] = block;

    delete registry[(unsigned char)nextID];

    block->blockID = nextID;
    registry[(unsigned char)nextID] = block;
    nameRegistry[id] = nextID;

    nextID++;
}

void BlockRegistry::registerBlockLua(const std::string &id, sol::table block) {
    registerBlock(id, new LuaBlock(block));
}

Block &BlockRegistry::getBlock(int id) {
    return *registry[(unsigned char)id];
}

Block& BlockRegistry::getBlock(const std::string &id) {
    if ( nameRegistry.find(id) == nameRegistry.end() ) {
        return getBlock(0);
    } else {
        return getBlock(nameRegistry[id]);
    }
}

int BlockRegistry::registeredBlocks() {
    return registry.size();
}