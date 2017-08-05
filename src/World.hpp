#pragma once

#include <map>
#include <vector>
#include <string>
#include <Math.hpp>

#include "sol.hpp"

class Texture;
class Shader;
class Camera;
class AABB;
class Chunk;


#define WORLD_SIZE 16
#define WORLD_HEIGHT 8

struct chunk_position {
    int x;
    int y;
    int z;

    //hashing algorithm stolen frome minetest
    bool operator<(const chunk_position& other) const {
        return 	(x<other.x && !(x == other.x)) ||
                ((x == other.x) && y<other.y && !(y == other.y)) ||
                ((x == other.x) && (y == other.y) && z<other.z && !(z == other.z));
    }
};

class World {
private:
    void updatePlayerActions(Camera &cam, float delta);

    bool isDedicatedServer;

public:
    sol::state luaState;

    std::map<chunk_position, Chunk*> chunks;

    ~World();

    World();

    void generate(bool empty);

    void addChunk(int x, int y, int z, Chunk *c);

    void generateNewChunk(int x, int y, int z);

    void deleteChunk(int x, int y, int z);

    void rebuild();

    void render(Camera &cam, Shader nearshader, Shader farShader, Texture tex);

    int getBlock(int x, int y, int z);

    Chunk *getChunk(int x, int y, int z);

    bool chunkExists(int x, int y, int z);

    void setBlock(int x, int y, int z, int block);

    void update(Camera &cam, float delta);

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
