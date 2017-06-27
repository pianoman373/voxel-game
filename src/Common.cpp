#include "Common.hpp"
#include "Block.hpp"

World Common::world;

void Common::init() {
    BlockRegistry::registerBlock(0, new BlockAir());
    BlockRegistry::registerBlock(1, new SimpleBlock(vec2i(0, 0)));
    BlockRegistry::registerBlock(2, new SimpleBlock(vec2i(4, 0)));
    BlockRegistry::registerBlock(3, new BlockGrass());
    BlockRegistry::registerBlock(4, new SimpleBlock(vec2i(2, 0)));

    world.generate();
}

void Common::update(float delta) {

}
