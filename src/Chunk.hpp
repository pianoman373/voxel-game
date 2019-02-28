#pragma once

#include <crucible/Mesh.hpp>
#include <crucible/AABB.hpp>
#include <crucible/Material.hpp>

#include "ChunkMesh.hpp"

#include <mutex>


class Chunk {
private:
    int getBlockFromWorld(int x, int y, int z);
	bool rebuild = false;


	Transform transform;


public:
	unsigned char blocks[16 * 16 * 256] = { 0 };
    bool isDirty = false;

    ChunkMesh mesh;
    bool empty = true;

    AABB aabb;

    int chunk_x;
    int chunk_z;

    /**
     * Serializes the chunk data into the provided array, and returns the actual length of the data.
     * The input array must be at least 32*32*32*2 elements.
     */
    int serialize(uint8_t *dataOut);

    /**
     * Updates this chunk with the specified serialized data.
     */
    void unSerialize(uint8_t *dataIn, int dataLength);

    Chunk(int x, int z);

    ~Chunk();

    char getBlock(int x, int y, int z);

    void setBlock(int x, int y, int z, char block);
};
