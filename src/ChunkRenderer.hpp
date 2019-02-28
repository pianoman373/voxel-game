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

    bool rebuild = false;

    Transform transform;


public:
    Chunk &attachedChunk;

    ChunkRenderer(Chunk &attachedChunk);

    char getBlock(ChunkRemeshJob &job, int x, int y, int z);

    void generateMesh(ChunkRemeshJob &job);

    void render(Material *mat);
};