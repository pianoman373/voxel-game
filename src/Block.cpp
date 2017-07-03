#include "Block.hpp"

//Block
Block::Block() {

}

vec2i Block::getTextureCoord(EnumDirection dir) {
    return vec2i(0, 0);
}

bool Block::isSolid() {
    return true;
}

//BlockRegistry
std::map<int, Block*> BlockRegistry::registry;

void BlockRegistry::registerBlock(int id, Block *block) {
    registry.insert(std::make_pair(id, block));
}

Block *BlockRegistry::getBlock(int id) {
    return registry[id];
}

int BlockRegistry::registeredBlocks() {
    return registry.size();
}

//SimpleBlock
SimpleBlock::SimpleBlock(vec2i tex, std::string name) {
    this->textureCoord = tex;
    this->name = name;
}

vec2i SimpleBlock::getTextureCoord(EnumDirection dir) {
    return textureCoord;
}

//BlockAir
BlockAir::BlockAir() {
    this->name = "Air";
}

vec2i BlockAir::getTextureCoord(EnumDirection dir) {
    return vec2i();
}

bool BlockAir::isSolid() {
    return false;
}

//BlockGrass
BlockGrass::BlockGrass() {
    this->name = "Grass";
}

vec2i BlockGrass::getTextureCoord(EnumDirection dir) {
    if (dir == EnumDirection::POSITIVE_Y) {
        return vec2i(3, 0);
    }
    else if (dir == EnumDirection::NEGATIVE_Y) {
        return vec2i(4, 0);
    }
    else {
        return vec2i(5, 0);
    }
}
