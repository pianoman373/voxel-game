#pragma once


#include <unordered_map>
#include <memory>

#include "common/Chunk.hpp"
#include "common/Player.hpp"
#include "client/ChunkRenderer.hpp"
#include "common/Block.hpp"
#include "common/Entity.hpp"
#include "common/ChunkNeighborhood.hpp"

class LuaHandler;

#define WORLD_SIZE 32

enum class EnumDirection {
    POSITIVE_X,
    NEGATIVE_X,
    POSITIVE_Y,
    NEGATIVE_Y,
    POSITIVE_Z,
    NEGATIVE_Z
};

class World {
private:
    std::function<void(std::shared_ptr<Chunk>)> deleteCallback;

    std::unordered_map<vec2i, std::shared_ptr<Chunk>> chunks;

    LuaHandler &lua;

    

public:
    std::vector<Entity*> entities;
    EntityRegistry entityRegistry;

    BlockRegistry blockRegistry;

    World(LuaHandler &lua);

    void tick();

    Block &getBlock(int x, int y, int z);

    void setBlockRaw(int x, int y, int z, int blockID);

    void setBlockFromString(int x, int y, int z, const std::string &block);

    void breakBlock(int x, int y, int z);

    virtual void setBlock(int x, int y, int z, Block &block);

    std::shared_ptr<Chunk> getChunk(int x, int z);

    ChunkNeighborhood getChunkNeighborhood(int x, int z);

    bool chunkExists(int x, int z);

    void deleteChunk(int x, int z);

    bool doesChunkHaveNeighbors(int x, int z);

    void updateNeighborsFlag(int x, int z);

    void informChunkChange(int x, int z);

    void deleteAllChunks();

    std::unordered_map<vec2i, std::shared_ptr<Chunk>> &getChunks();

    /**
     * Returns the nearest block the specified ray intercepts.
     * blockPosReturn is a passthrough parameter which returns the block position of the intercepted block.
     * blockNormalReturn is the direction (normal) of the block face the ray intercepted.
     *
     * Returns true if a block collision was found.
     */
    bool raycastBlocks(vec3 origin, vec3 direction, float maxDistance, vec3i &blockPosReturn, vec3i &blockNormalReturn);

    /**
     * Returns all AABB's in the world that collide with the given test AABB
     */
    std::vector<AABB> getCollisions(AABB test);

    void setDeleteCallback(std::function<void(std::shared_ptr<Chunk>)> deleteCallback);

    void spawnEntity(const std::string &id, float x, float y, float z);
};