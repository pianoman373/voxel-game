#pragma once

#include <crucible/Math.hpp>
#include <map>
#include <string>

#include "sol.hpp"

class Block {
    friend class BlockRegistry;

private:
    int blockID;

public:
    std::string name = "";
    vec3 color;

    Block();

    virtual vec2i getTextureCoord(EnumDirection dir);

    virtual bool isSolid();

    int getID();
};

class LuaBlock : public Block {
    std::vector<vec2i> textureCoords;
    bool solid;

public:
    LuaBlock(sol::table table);

    vec2i getTextureCoord(EnumDirection dir);

    bool isSolid();

};

class BlockRegistry {
private:
    std::map<int, Block*> registry;

public:

    void registerBlock(int id, Block *block);

    void registerBlockLua(int id, sol::table block);

    Block &getBlock(int id);

    int registeredBlocks();
};