#include "common/World.hpp"

#include "rendering/Renderer.hpp"
#include "rendering/IBL.hpp"
#include "common/LuaHandler.hpp"





World::World(LuaHandler &lua): lua(lua) {

}

void World::tick() {
    for (auto *i : entities) {
        i->tick();
    }
}

Block &World::getBlock(int x, int y, int z) {
    int xp = x >> 4;
    int zp = z >> 4;

    std::shared_ptr<Chunk> c = getChunk(xp, zp);

    return c->getBlock(x & 15, y, z & 15);
}

void World::setBlockRaw(int x, int y, int z, int blockID) {
    int xp = x >> 4;
    int zp = z >> 4;

    int xc = x & 15;
    int yc = y;
    int zc = z & 15;

    std::shared_ptr<Chunk> c = getChunk(xp, zp);

    c->setBlockRaw(xc, yc, zc, blockID);
}

void World::setBlockFromString(int x, int y, int z, const std::string &block) {
    setBlock(x, y, z, blockRegistry.getBlock(block));
}

void World::breakBlock(int x, int y, int z) {
    setBlock(x, y, z, blockRegistry.getBlock(0));
}

void World::setBlock(int x, int y, int z, Block &block) {
    int xp = x >> 4;
    int zp = z >> 4;

    int xc = x & 15;
    int yc = y;
    int zc = z & 15;

    std::shared_ptr<Chunk> c = getChunk(xp, zp);

    c->setBlock(xc, yc, zc, block);
    c->isDirty = true;


    if (xc == 15 && chunkExists(xp + 1, zp)) {
        getChunk(xp + 1, zp)->isDirty = true;
    }
    if (xc == 0 && chunkExists(xp - 1, zp)) {
        getChunk(xp - 1, zp)->isDirty = true;
    }

    if (zc == 15 && chunkExists(xp, zp + 1)) {
        getChunk(xp, zp + 1)->isDirty = true;
    }
    if (zc == 0 && chunkExists(xp, zp - 1)) {
        getChunk(xp, zp - 1)->isDirty = true;
    }
    
}

std::shared_ptr<Chunk> World::getChunk(int x, int z) {
    if (!chunkExists(x, z)) {
        Chunk *c = new Chunk(*this, x, z);

        chunks.emplace(vec2i(x, z), c);
        informChunkChange(x, z);
    }

    return chunks[{x, z}];
}

ChunkNeighborhood World::getChunkNeighborhood(int x, int z) {
    ChunkNeighborhood neighborhood(
            getChunk(x, z),

            getChunk(x+1, z),
            getChunk(x-1, z),

            getChunk(x, z+1),
            getChunk(x, z-1),

            getChunk(x+1, z+1),
            getChunk(x+1, z-1),

            getChunk(x-1, z+1),
            getChunk(x-1, z-1)
    );

    return neighborhood;
}

void World::deleteChunk(int x, int z) {
    if (chunkExists(x, z)) {
        if (deleteCallback) {
            deleteCallback(getChunk(x, z));
        }

        chunks.erase({x, z});
    }
    informChunkChange(x, z);
}

bool World::doesChunkHaveNeighbors(int x, int z) {
    bool posX = chunkExists(x+1, z);
    bool negX = chunkExists(x-1, z);

    bool posZ = chunkExists(x, z+1);
    bool negZ = chunkExists(x, z-1);

    bool posXposZ = chunkExists(x+1, z+1);
    bool posXnegZ = chunkExists(x+1, z-1);

    bool negXposZ = chunkExists(x-1, z+1);
    bool negXnegZ = chunkExists(x-1, z-1);

    if (posX && negX && posZ && negZ && posXposZ && posXnegZ && negXposZ && negXnegZ) {
        return true;
    }
    else {
        return false;
    }
}

void World::updateNeighborsFlag(int x, int z) {
    if (chunkExists(x, z)) {
        getChunk(x, z)->hasNeighbors = doesChunkHaveNeighbors(x, z);
    }
}

void World::informChunkChange(int x, int z) {
    updateNeighborsFlag(x+1, z);
    updateNeighborsFlag(x-1, z);
    updateNeighborsFlag(x, z+1);
    updateNeighborsFlag(x, z-1);

    updateNeighborsFlag(x-1, z-1);
    updateNeighborsFlag(x+1, z-1);
    updateNeighborsFlag(x-1, z+1);
    updateNeighborsFlag(x+1, z+1);
}

void World::deleteAllChunks() {
    std::vector<vec2i> chunkPositions;

    for (auto const &ref: chunks) {
        chunkPositions.push_back(ref.first);
    }

    for (int i = 0; i < chunkPositions.size(); i++) {
        deleteChunk(chunkPositions[i].x, chunkPositions[i].y);
    }
}

bool World::chunkExists(int x, int z) {
    bool exists = chunks.find({x, z}) != chunks.end();

    return exists;
}

std::unordered_map<vec2i, std::shared_ptr<Chunk>> &World::getChunks() {
    //chunks_mx.lock();
    //std::unordered_map<vec2i, std::shared_ptr<Chunk>> ret = chunks;
    //chunks_mx.unlock();

    return chunks;
}

bool World::raycastBlocks(vec3 origin, vec3 direction, float maxDistance, vec3i &blockPosReturn, vec3i &blockNormalReturn) {
    int xp = (int)origin.x >> 4;
    int zp = (int)origin.z >> 4;

    vec3 p0 = origin;
    vec3 d = direction;

    float epsilon = 0.01f;

    float t = 0;
    while (t <= maxDistance) {
        vec3 p = p0 + d * t;

        vec3i pi = vec3i((int)p.x, (int)p.y, (int)p.z);

        if (getBlock(pi.x, pi.y, pi.z).getID() != 0) {
            vec3 n = (p - (vec3(pi.x, pi.y, pi.z) + vec3(0.5))) * 2.0f;
            vec3i normal;

            if (abs(n.x) > abs(n.y) && abs(n.x) > abs(n.z)) {
                if (n.x > 0) {
                    normal = vec3i(1, 0, 0);
                }
                else {
                    normal = vec3i(-1, 0, 0);
                }
                
            }

            if (abs(n.y) > abs(n.x) && abs(n.y) > abs(n.z)) {
                if (n.y > 0) {
                    normal = vec3i(0, 1, 0);
                }
                else {
                    normal = vec3i(0, -1, 0);
                }
                
            }

            if (abs(n.z) > abs(n.y) && abs(n.z) > abs(n.x)) {
                if (n.z > 0) {
                    normal = vec3i(0, 0, 1);
                }
                else {
                    normal = vec3i(0, 0, -1);
                }
                
            }

            blockNormalReturn = normal;
            blockPosReturn = pi;
            return true;
        }
    
        vec3 deltas = (step(0, d) - fract(p)) / d;
        t += fmax(mincomp(deltas), epsilon);
    }

    return false;
}

std::vector<AABB> World::getCollisions(AABB test) {
    test.min = test.min - 1.0f;
    test.max = test.max + 1.0f;
    vec3i start = vec3i(floor(test.min.x), floor(test.min.y), floor(test.min.z));
    vec3i end = vec3i(ceil(test.max.x), ceil(test.max.y), ceil(test.max.z));

    std::vector<AABB> returnVector;

    //return returnVector;

    for (int x = start.x; x <= end.x; x++) {
        for (int y = start.y; y <= end.y; y++) {
            for (int z = start.z; z <= end.z; z++) {
                int xp = x >> 4;
                int zp = z >> 4;

                if (getBlock(x, y, z).isSolid()) {
                        AABB blockAbb = AABB(vec3(x, y, z), vec3(x+1, y+1, z+1));

                        returnVector.push_back(blockAbb);
                    }
            }
        }
    }

    return returnVector;
}

void World::setDeleteCallback(std::function<void(std::shared_ptr<Chunk>)> deleteCallback) {
    this->deleteCallback = deleteCallback;
}

void World::spawnEntity(const std::string &id, float x, float y, float z) {
    entities.push_back(new Entity(*this, lua, entityRegistry.getEntityTable(id), vec3(x, y, z)));
}