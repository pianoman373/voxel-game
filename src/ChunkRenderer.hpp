#pragma once

#include "ChunkMesh.hpp"
#include "World.hpp"

#include "core/AABB.hpp"
#include "core/Material.hpp"

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

    void generateMesh(ChunkNeighborhood &neighborhood);

    void render(Material *mat);
};