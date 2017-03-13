#pragma once

#include "Chunk.hpp"
#include <map>

struct chunk_position {
    int x;
    int y;
    int z;

    bool operator<(const chunk_position& rhs) const {
        return (x + y + z) < (rhs.x + rhs.y + rhs.z);
    }
};

class World {
private:
    std::map<chunk_position, Chunk*> chunks;

public:
    void addChunk(int x, int y, int z, Chunk *c);

    void deleteChunk(int x, int y, int z);

    void render();
};
