#pragma once

#include <crucible/Math.hpp>
#include <crucible/AABB.hpp>

#include <unordered_map>
#include <mutex>

#include "Chunk.hpp"


// stuff to make sure a vec3i can be used in an unorderered:map
struct key_hash : public std::unary_function<vec3i, std::size_t>
{
    std::size_t operator()(const vec3i& k) const
    {
        return k.x ^ k.y ^ k.z;
    }
};

struct key_equal : public std::binary_function<vec3i, vec3i, bool>
{
    bool operator()(const vec3i& v0, const vec3i& v1) const
    {
        return (
                v0.x == v1.x &&
                v0.y == v1.y &&
                v0.z == v1.z
        );
    }
};
//--------------------------------------------------------------

class ChunkManager {
private:
    std::unordered_map<vec3i, Chunk*, key_hash, key_equal> chunks;
    std::mutex chunks_mx;
public:

    ChunkManager();

    int getBlock(int x, int y, int z);

    void setBlock(int x, int y, int z, int block);

    Chunk *getChunk(int x, int y, int z);

    bool chunkExists(int x, int y, int z);

    void deleteChunk(int x, int y, int z);

    std::unordered_map<vec3i, Chunk*, key_hash, key_equal> getChunks();

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