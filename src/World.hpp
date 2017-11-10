#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <crucible/Math.hpp>
#include <functional>
#include <mutex>

#include "sol.hpp"

class Texture;
class Shader;
class Camera;
class AABB;
class Chunk;
class Material;


#define WORLD_SIZE 16
#define WORLD_HEIGHT 8

struct chunk_position {
    int x;
    int y;
    int z;

};

struct key_hash : public std::unary_function<chunk_position, std::size_t>
{
    std::size_t operator()(const chunk_position& k) const
    {
        return k.x ^ k.y ^ k.z;
    }
};

struct key_equal : public std::binary_function<chunk_position, chunk_position, bool>
{
    bool operator()(const chunk_position& v0, const chunk_position& v1) const
    {
        return (
                v0.x == v1.x &&
                v0.y == v1.y &&
                v0.z == v1.z
        );
    }
};

class World {
private:
    void updatePlayerActions(Camera &cam, float delta);

    bool isDedicatedServer;
    std::vector<vec3i> chunkLoadingPositions;

    std::vector<Chunk*> chunksToUpdate;
    std::mutex chunksToUpdateMutex;

public:
    sol::state luaState;

    std::unordered_map<chunk_position, Chunk*, key_hash, key_equal> chunks;

    ~World();

    World();

    void addChunk(int x, int y, int z, Chunk *c);

    void generateNewChunk(int x, int y, int z);

    void deleteChunk(int x, int y, int z);

    void rebuild();

    void render(Camera &cam, Material *nearmaterial, Material *farmaterial);

    int getBlock(int x, int y, int z);

    Chunk *getChunk(int x, int y, int z);

    bool chunkExists(int x, int y, int z);

    void setBlock(int x, int y, int z, int block);

    void notifyChunkChange(int x, int y, int z);

    void update(const Camera &cam, float delta);

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

    static vec3i worldToChunkPos(vec3 pos);
};
