#pragma once

#include "Math.hpp"

#include <map>
#include <string>

class Block {
public:
    std::string name = "";
    vec3 color;

    Block();

    virtual vec2i getTextureCoord(EnumDirection dir);

    virtual bool isSolid();
};

class LuaBlock : public Block {
    int blockID;
    bool solid;

public:
    LuaBlock(int blockID);

    vec2i getTextureCoord(EnumDirection dir);

    bool isSolid();
};

class BlockRegistry {
private:
    static std::map<int, Block*> registry;

public:
    static void registerBlock(int id, Block *block);

    static Block *getBlock(int id);

    static int registeredBlocks();
};
