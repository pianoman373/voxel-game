#include "ChunkManager.hpp"

#include <crucible/Renderer.hpp>

ChunkManager::ChunkManager() {

}

int ChunkManager::getBlock(int x, int y, int z) {
    int xp = x >> 4;
    int zp = z >> 4;

    Chunk *c = getChunk(xp, zp);

    return c->getBlock(x & 15, y, z & 15);
}

void ChunkManager::setBlock(int x, int y, int z, int block) {
    int xp = x >> 4;
    int zp = z >> 4;

    int xc = x & 15;
    int yc = y;
    int zc = z & 15;

    Chunk *c = getChunk(xp, zp);

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

Chunk *ChunkManager::getChunk(int x, int z) {
    if (!chunkExists(x, z)) {
        Chunk *c = new Chunk(x, z);

        chunks_mx.lock();
        chunks.emplace(vec2i(x, z), c);
        chunks_mx.unlock();
    }

    return chunks[{x, z}];
}

void ChunkManager::deleteChunk(int x, int z) {
    if (chunkExists(x, z)) {
        //std::cout << "deleting chunk" << std::endl;

        chunks_mx.lock();
        delete(chunks[{x, z}]);

        chunks.erase({x, z});
        chunks_mx.unlock();
    }
}

void ChunkManager::shutdown() {
    chunks.clear();
}

bool ChunkManager::chunkExists(int x, int z) {
    bool exists = chunks.find({x, z}) != chunks.end();

    return exists;
}

std::unordered_map<vec2i, Chunk*, key_hash, key_equal> ChunkManager::getChunks() {
    //chunks_mx.lock();
    std::unordered_map<vec2i, Chunk*, key_hash, key_equal> ret = chunks;
    //chunks_mx.unlock();

    return ret;
}

bool ChunkManager::raycastBlocks(vec3 origin, vec3 direction, float maxDistance, vec3i &blockPosReturn, vec3 &blockNormalReturn) {
    Chunk *nearestBlockChunk;
    vec3i nearestBlockPos;
    float nearestBlockDistance = 10000000000000.0f;
    vec3 nearestBlockNormal;

    for (auto const &ref: chunks) {
        Chunk *c = ref.second;

        if (!c->empty) {
            vec3 chunkPos = vec3(c->chunk_x * 16, 0, c->chunk_z * 16);

            AABB abb = AABB(chunkPos, chunkPos + vec3(16, 256, 16));

            vec3 point;
            vec3 normal;

            //get raycasts
            if (abb.raycast(origin, direction * maxDistance, point, normal)) {
                for (int x = 0; x < 16; x++) {
                    for (int y = 0; y < 256; y++) {
                        for (int z = 0; z < 16; z++) {
                            char block = c->getBlock(x, y, z);
                            if (block != 0) {
                                AABB blockAbb = AABB(chunkPos + vec3(x, y, z), chunkPos + vec3(x + 1, y + 1, z + 1));

                                if (blockAbb.raycast(origin, direction * maxDistance, point, normal)) {
                                    float distance = length(origin - point);

                                    if (distance < nearestBlockDistance) {
                                        nearestBlockDistance = distance;
                                        nearestBlockChunk = c;
                                        nearestBlockPos = vec3i(x, y, z);
                                        nearestBlockNormal = normal;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (nearestBlockDistance != 10000000000000.0f) {
        vec3i chunkPos = vec3i(nearestBlockChunk->chunk_x * 16, 0, nearestBlockChunk->chunk_z * 16);

        blockNormalReturn = nearestBlockNormal;
        blockPosReturn = nearestBlockPos + chunkPos;
        return true;
    }
    return false;
}

std::vector<AABB> ChunkManager::getCollisions(AABB test) {

    vec3i start = vec3i(test.min.x, test.min.y, test.min.z);
    vec3i end = vec3i(test.max.x, test.max.y, test.max.z);

    std::vector<AABB> returnVector;

    for (int x = start.x; x <= end.x; x++) {
        for (int y = start.y; y <= end.y; y++) {
            for (int z = start.z; z <= end.z; z++) {
                if (getBlock(x, y, z) != 0) {
                    AABB blockAbb = AABB(vec3(x, y, z), vec3(x+1, y+1, z+1));

                    returnVector.push_back(blockAbb);
                }

            }
        }
    }

    return returnVector;
}