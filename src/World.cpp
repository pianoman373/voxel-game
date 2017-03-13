#include "World.hpp"

void World::addChunk(int x, int y, int z, Chunk *c) {
    chunk_position pos = {x, y, z};
    chunks.insert(std::make_pair(pos, c));
}

void World::deleteChunk(int x, int y, int z) {
    chunk_position pos = {x, y, z};
    Chunk *c = chunks.at(pos);
}

void World::render() {

}
