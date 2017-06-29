#pragma once

#include <map>
#include <unordered_map>
#include <vector>
#include <string>

#include "Math.hpp"
#include "Texture.hpp"
#include "Camera.hpp"
#include "Player.hpp"
#include "Chunk.hpp"
#include "AABB.hpp"
#include "Shader.hpp"

#define WORLD_SIZE 8
#define WORLD_HEIGHT 2

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
    std::map<chunk_position, Chunk*> chunks;

    ~World();

    World();

    void generate();

    void addChunk(int x, int y, int z, Chunk *c);

    void deleteChunk(int x, int y, int z);

    void rebuild();

    void render(Camera &cam, Shader shader, Texture tex);

    int getBlock(int x, int y, int z);

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
