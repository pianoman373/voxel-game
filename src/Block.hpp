#pragma once

#include <crucible/Math.hpp>
#include <map>
#include <string>

#include "sol.hpp"

class Block {
    friend class BlockRegistry;

private:
    int blockID = 0;

public:
    std::string name = "";
    vec3 color;

    Block();

    virtual vec2i getTextureCoord(EnumDirection dir);

    virtual bool isSolid();

    virtual int getLightLevel();

    int getID();
};

class LuaBlock : public Block {
    std::vector<vec2i> textureCoords;
    bool solid;
    int lightLevel;

public:
    LuaBlock(sol::table table);

    vec2i getTextureCoord(EnumDirection dir);

    bool isSolid();

    int getLightLevel();

};

class BlockRegistry {
private:
    std::vector<Block*> registry;
    std::map<std::string, int> nameRegistry;
    int nextID = 1;

public:
    BlockRegistry();

    void registerBlock(const std::string &id, Block *block);

    void registerBlockLua(const std::string &id, sol::table block);

    Block &getBlock(int id);

    Block &getBlock(const std::string &id);

    int registeredBlocks();
};