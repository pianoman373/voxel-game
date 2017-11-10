#pragma once

#define CHUNK_SIZE 32

#include <crucible/Mesh.hpp>
#include <crucible/MeshFactory.hpp>

class World;


class Chunk {
private:

    World *world;

    int getBlockFromWorld(int x, int y, int z);

public:
    char blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];

    Mesh mesh;
    MeshFactory ms;
    bool empty = true;

    //TODO: should be a vec3i
    int chunk_x;
    int chunk_y;
    int chunk_z;

    bool rebuild = true;

    Chunk(int x, int y, int z, World* world);

    ~Chunk();

    char getBlock(int x, int y, int z);

    void setBlock(int x, int y, int z, char block);

    /**
     * Regenerates what should actually be rendered. Required for block
     * changes to appear.
     */
    void generateMesh();
};
