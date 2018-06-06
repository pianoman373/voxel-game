#pragma once

#include <crucible/Math.hpp>
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

class SimpleBlock : public Block {
    vec2i textureCoord;
    bool solid;

public:
    SimpleBlock(vec2i textureCoord, std::string name, bool solid);

    vec2i getTextureCoord(EnumDirection dir);

    bool isSolid();
};

class GrassBlock : public Block {
public:
    GrassBlock();

    vec2i getTextureCoord(EnumDirection dir);
};

class BlockRegistry {
private:
    static std::map<int, Block*> registry;

public:
    static void registerBlock(int id, Block *block);

    static Block *getBlock(int id);

    static int registeredBlocks();
};
