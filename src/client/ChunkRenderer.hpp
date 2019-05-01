#pragma once

#include "client/ChunkMesh.hpp"
#include "common/World.hpp"

#include "util/AABB.hpp"
#include "rendering/Material.hpp"

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