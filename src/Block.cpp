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

//SimpleBlock

SimpleBlock::SimpleBlock(vec2i textureCoord, std::string name, bool solid) {
    this->textureCoord = textureCoord;
    this->name = name;
    this->solid = solid;
}

vec2i SimpleBlock::getTextureCoord(EnumDirection dir) {
    return textureCoord;
}

bool SimpleBlock::isSolid() {
    return solid;
}

//GrassBlock

GrassBlock::GrassBlock() {
    this->name = "Grass";
}

vec2i GrassBlock::getTextureCoord(EnumDirection dir) {
    if (dir == EnumDirection::POSITIVE_Y) {
        return {0, 0};
    }
    if (dir == EnumDirection::NEGATIVE_Y) {
        return {2, 0};
    }
    return {3, 0};

}

//BlockRegistry
std::map<int, Block*> BlockRegistry::registry;

void BlockRegistry::registerBlock(int id, Block *block) {
    registry.insert(std::make_pair(id, block));
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
