#pragma once

#include <crucible/Mesh.hpp>
#include <crucible/AABB.hpp>
#include <crucible/Material.hpp>

#include "ChunkMesh.hpp"
#include "Block.hpp"

#include <queue>
#include <mutex>

class ChunkNeighborhood;
class World;

class Chunk {
private:
    int getBlockFromWorld(int x, int y, int z);
	bool rebuild = false;


	Transform transform;
	World &world;


public:
	unsigned char blocks[16 * 16 * 256] = { 0 };
	unsigned char lightMap[16 * 16 * 256] = { 0 };
    bool isDirty = false;

    ChunkMesh mesh;

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

    Chunk(World &world, int x, int z);

    ~Chunk();

    Block &getBlock(int x, int y, int z);

    void setBlock(int x, int y, int z, Block &block);

	int getSunlight(int x, int y, int z);

	void setSunlight(int x, int y, int z, int val);

	int getTorchlight(int x, int y, int z);

	void setTorchlight(int x, int y, int z, int val);

	void calculateSunLighting();

	void propagateSunlight(ChunkNeighborhood &neighborhood, std::queue<vec3i> &lightBfsQueue);

	void unPropagateSunlight(ChunkNeighborhood &neighborhood, std::queue<vec4i> &lightRemovalQueue);

    void propagateTorchlight(ChunkNeighborhood &neighborhood, std::queue<vec3i> &lightBfsQueue);

    void unPropagateTorchlight(ChunkNeighborhood &neighborhood, std::queue<vec4i> &lightRemovalQueue);
};
