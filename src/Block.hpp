#pragma once

#include <crucible/Math.hpp>
#include <map>
#include <string>

#include "sol.hpp"

class Block {
public:
    std::string name = "";
    vec3 color;

    Block();

    virtual vec2i getTextureCoord(EnumDirection dir);

    virtual bool isSolid();
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
    //static std::map<int, Block*> registry;
    static std::vector<Block*> registry;

public:
    static void init();

    static void registerBlock(int id, Block *block);

    static void registerBlockLua(int id, sol::table block);

    static Block *getBlock(int id);

    static int registeredBlocks();
};