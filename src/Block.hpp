#pragma once

#include "Math.hpp"

#include <map>

class Block {
public:
    Block();

    virtual vec2i getTextureCoord(EnumDirection dir);

    virtual bool isSolid();
};

class BlockRegistry {
private:
    static std::map<int, Block*> registry;

public:
    static void registerBlock(int id, Block *block);

    static Block *getBlock(int id);
};

class SimpleBlock : public Block {
private:
    vec2i textureCoord;

public:
    SimpleBlock(vec2i tex);

    vec2i getTextureCoord(EnumDirection dir);
};

class BlockAir : public Block {
public:
    BlockAir();

    vec2i getTextureCoord(EnumDirection dir);

    bool isSolid();
};

class BlockGrass : public Block {
public:
    BlockGrass();

    vec2i getTextureCoord(EnumDirection dir);
};
