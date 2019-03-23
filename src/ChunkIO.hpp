#pragma once

#include "Chunk.hpp"
#include <memory>

class ChunkIO {
private:
    uint8_t *rleCache;

public:
    ChunkIO();

    ~ChunkIO();

    void saveChunk(std::shared_ptr<Chunk> chunk);

    void loadChunk(std::shared_ptr<Chunk> chunk);
};