#include "common/Block.hpp"
#include "client/Client.hpp"
#include "common/World.hpp"

#include "common/LuaHandler.hpp"

static Block defaultBlock;

//Block
Block::Block(sol::table table) {
    this->table = table;
    name = table["name"].get_or(std::string("Untitled"));
    solid = table["solid"].get_or(true);
    lightLevel = table["lightLevel"].get_or(0);
    isLiquid = table["isLiquid"].get_or(false);
    color = table["color"].get_or(0);

    for (int i = 0; i < 6; i++) {
        textureIndices.push_back(0);
    }
}

Block::Block() {
    name = "null";
    stringID = "null";
}

void Block::clientInit(Client &client) {
    if (table != sol::nil) {
        if (table["textures"].get_type() == sol::type::table) {
            sol::table textures = table["textures"];
        
            if (textures.size() == 1) {
                for (int i = 0; i < 6; i++) {
                    textureIndices[i] = client.worldRenderer.atlas.registerTexture(LuaHandler::formatModPath(textures[1]));
                }
            }
            else if (textures.size() == 6) {
                for (int i = 0; i < 6; i++) {
                    textureIndices[i] = client.worldRenderer.atlas.registerTexture(LuaHandler::formatModPath(textures[i+1]));
                }
            }
        }
        
    }
}

int Block::getTextureIndex(EnumDirection dir) {
    return textureIndices[static_cast<int>(dir)];
}

bool Block::isSolid() {
    return solid;
}

int Block::getLightLevel() {
    return lightLevel;
}

int Block::getID() {
    return blockID;
}

std::string &Block::getStringID() {
    return stringID;
}

void Block::onPlace(World &world, int x, int y, int z) {
    if (table != sol::nil) {
        LuaHandler::safeCall(table.get<sol::safe_function>("onPlace"), world, x, y, z);
    }
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
    block->stringID = id;
    registry[(unsigned char)nextID] = block;
    nameRegistry[id] = nextID;

    nextID++;
}

void BlockRegistry::registerBlockLua(const std::string &id, sol::table block) {
    registerBlock(id, new Block(block));
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

void BlockRegistry::clientInit(Client &client) {
    for (int i = 0; i < registry.size(); i++) {
        registry[i]->clientInit(client);
    }
}