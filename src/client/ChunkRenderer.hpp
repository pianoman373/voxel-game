#pragma once

#include "client/ChunkMesh.hpp"
#include "common/World.hpp"

#include "util/AABB.hpp"
#include "rendering/Material.hpp"

#include <memory>

class Chunk;
struct ChunkRemeshJob;

class ChunkRenderer {
private:
    ChunkMesh mesh;
    ChunkMesh liquidMesh;

    AABB aabb;

    bool updateMesh = false;

    Transform transform;

    


public:
    long rendererIndex = 0;

    std::shared_ptr<Chunk> chunk;

    int chunk_x;
    int chunk_z;

    ChunkRenderer(int chunk_x, int chunk_z, long rendererIndex, std::shared_ptr<Chunk> chunk);

    void recycle(int chunk_x, int chunk_z, long rendererIndex, std::shared_ptr<Chunk> chunk);

    void generateMesh(ChunkNeighborhood &neighborhood);

    void render(Material *mat, Material *liquidMat);

    void clear();
};