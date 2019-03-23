#include "ChunkIO.hpp"

#include <fstream>
#include <iostream>

ChunkIO::ChunkIO() {
    rleCache = new uint8_t[16*16*256*5];
}

ChunkIO::~ChunkIO() {
    delete[] rleCache;
}

void ChunkIO::saveChunk(std::shared_ptr<Chunk> chunk) {
    std::string filename = "save/" + std::to_string(chunk->chunk_x) + "_" + std::to_string(chunk->chunk_z) + ".chunk";

    std::ofstream s(filename, std::ios::binary);

    uint16_t length = chunk->serialize(rleCache);

    s.write((char*)rleCache, length);

    s.close();
}

void ChunkIO::loadChunk(std::shared_ptr<Chunk> chunk) {
    std::string filename = "save/" + std::to_string(chunk->chunk_x) + "_" + std::to_string(chunk->chunk_z) + ".chunk";

    std::ifstream s(filename, std::ios::binary);

    int i;
    char buf;
    while (s.read(&buf, 1))
    {
        rleCache[i] = (uint8_t)buf;

        i++;
    }

    chunk->unSerialize(rleCache, i);

    s.close();
}