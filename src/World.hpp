#pragma once


#include <unordered_map>
#include <memory>

#include "Chunk.hpp"
#include "Player.hpp"
#include "ChunkRenderer.hpp"

enum Context {
    CLIENT,
    SERVER,
    COMMON
};

#define WORLD_SIZE 32

struct ChunkNeighborhood {
    std::shared_ptr<Chunk> center;

    std::shared_ptr<Chunk> posX;
    std::shared_ptr<Chunk> negX;

    std::shared_ptr<Chunk> posZ;
    std::shared_ptr<Chunk> negZ;

    std::shared_ptr<Chunk> posXposZ;
    std::shared_ptr<Chunk> posXnegZ;

    std::shared_ptr<Chunk> negXposZ;
    std::shared_ptr<Chunk> negXnegZ;

    char getBlock(int x, int y, int z);

    int getSunlight(int x, int y, int z);

    void setBlock(int x, int y, int z, char block);

    void setSunlight(int x, int y, int z, int val);
};


class World {
private:
    Context ctx;


    std::unordered_map<vec2i, std::shared_ptr<Chunk>> chunks;

public:
    World();

    void init(Context ctx);

    void update(float delta);

    int getBlock(int x, int y, int z);

    void setBlock(int x, int y, int z, int block);

    std::shared_ptr<Chunk> getChunk(int x, int z);

    ChunkNeighborhood getChunkNeighborhood(int x, int z);

    bool chunkExists(int x, int z);

    void deleteChunk(int x, int z);

    void shutdown();

    std::unordered_map<vec2i, std::shared_ptr<Chunk>> getChunks();

    /**
     * Returns the nearest block the specified ray intercepts.
     * blockPosReturn is a passthrough parameter which returns the block position of the intercepted block.
     * blockNormalReturn is the direction (normal) of the block face the ray intercepted.
     *
     * Returns true if a block collision was found.
     */
    bool raycastBlocks(vec3 origin, vec3 direction, float maxDistance, vec3i &blockPosReturn, vec3 &blockNormalReturn);

    /**
     * Returns all AABB's in the world that collide with the given test AABB
     */
    std::vector<AABB> getCollisions(AABB test);
};