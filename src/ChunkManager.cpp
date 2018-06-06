#include "ChunkManager.hpp"

#include <crucible/Renderer.hpp>

ChunkManager::ChunkManager() {

}

int ChunkManager::getBlock(int x, int y, int z) {
    int xp = x >> 5;
    int yp = y >> 5;
    int zp = z >> 5;

    Chunk *c = getChunk(xp, yp, zp);

    return c->getBlock(x & 31, y & 31, z & 31);
}

void ChunkManager::setBlock(int x, int y, int z, int block) {
    int xp = x >> 5;
    int yp = y >> 5;
    int zp = z >> 5;

    int xc = x & 31;
    int yc = y & 31;
    int zc = z & 31;

    Chunk *c = getChunk(xp, yp, zp);

    c->setBlock(xc, yc, zc, block);
    c->isDirty = true;


    if (xc == 31 && chunkExists(xp + 1, yp, zp)) {
        getChunk(xp + 1, yp, zp)->isDirty = true;
    }
    if (xc == 0 && chunkExists(xp - 1, yp, zp)) {
        getChunk(xp - 1, yp, zp)->isDirty = true;
    }

    if (yc == 31 && chunkExists(xp, yp + 1, zp)) {
        getChunk(xp, yp + 1, zp)->isDirty = true;
    }
    if (yc == 0 && chunkExists(xp, yp - 1, zp)) {
        getChunk(xp, yp - 1, zp)->isDirty = true;
    }

    if (zc == 31 && chunkExists(xp, yp, zp + 1)) {
        getChunk(xp, yp, zp + 1)->isDirty = true;
    }
    if (zc == 0 && chunkExists(xp, yp, zp - 1)) {
        getChunk(xp, yp, zp - 1)->isDirty = true;
    }
}

Chunk *ChunkManager::getChunk(int x, int y, int z) {
    if (!chunkExists(x, y, z)) {
        Chunk *c = new Chunk(x, y, z, this);

        chunks_mx.lock();
        chunks.emplace(vec3i(x, y, z), c);
        chunks_mx.unlock();
    }

    return chunks[{x, y, z}];
}

void ChunkManager::deleteChunk(int x, int y, int z) {
    if (chunkExists(x, y, z)) {
        chunks_mx.lock();
        delete(chunks[{x, y, z}]);

        chunks.erase({x, y, z});
        chunks_mx.unlock();
    }

}

bool ChunkManager::chunkExists(int x, int y, int z) {
    chunks_mx.lock();
    bool exists = chunks.find({x, y, z}) != chunks.end();
    chunks_mx.unlock();

    return exists;
}

std::unordered_map<vec3i, Chunk*, key_hash, key_equal> ChunkManager::getChunks() {
    chunks_mx.lock();
    std::unordered_map<vec3i, Chunk*, key_hash, key_equal> ret = chunks;
    chunks_mx.unlock();

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
            vec3 chunkPos = vec3(c->chunk_x * CHUNK_SIZE, (c->chunk_y * CHUNK_SIZE), c->chunk_z * CHUNK_SIZE);

            AABB abb = AABB(chunkPos, chunkPos + vec3(CHUNK_SIZE));

            vec3 point;
            vec3 normal;

            //get raycasts
            if (abb.raycast(origin, direction * maxDistance, point, normal)) {
                for (int x = 0; x < CHUNK_SIZE; x++) {
                    for (int y = 0; y < CHUNK_SIZE; y++) {
                        for (int z = 0; z < CHUNK_SIZE; z++) {
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
        vec3i chunkPos = vec3i(nearestBlockChunk->chunk_x * CHUNK_SIZE, (nearestBlockChunk->chunk_y * CHUNK_SIZE), nearestBlockChunk->chunk_z * CHUNK_SIZE);

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