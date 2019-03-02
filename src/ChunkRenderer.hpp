#pragma once

#include "ChunkMesh.hpp"

#include <crucible/AABB.hpp>
#include <crucible/Material.hpp>

class Chunk;
struct ChunkRemeshJob;

class ChunkRenderer {
private:
    ChunkMesh mesh;

    AABB aabb;

    bool updateMesh = false;

    Transform transform;

    int chunk_x;
    int chunk_z;


public:

    ChunkRenderer(int chunk_x, int chunk_z);

    char getBlock(ChunkRemeshJob &job, int x, int y, int z);

    void generateMesh(ChunkRemeshJob &job);

    void render(Material *mat);
};