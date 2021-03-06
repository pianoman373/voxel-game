#pragma once

#include "Math.hpp"
#include <map>
#include <string>

#include "sol.hpp"

class World;
enum class EnumDirection;
class Client;

class Block {
    friend class BlockRegistry;

private:
    

    int blockID = 0;
    std::string name = "";
    std::string stringID;
    
    std::vector<vec2i> textureCoords;
    std::vector<int> textureIndices;
    bool solid = false;
    int lightLevel = 0;

public:
    sol::table table = sol::nil;
    bool isLiquid = false;
    uint32_t color;


    Block(sol::table table);

    Block();

    void clientInit(Client &client);

    int getTextureIndex(EnumDirection dir);

    bool isSolid();

    int getLightLevel();

    int getID();

    std::string &getStringID();

    void onPlace(World &world, int x, int y, int z);
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

    void clientInit(Client &client);
};