#include "World.hpp"

#include <crucible/Renderer.hpp>
#include <crucible/IBL.hpp>

char ChunkNeighborhood::getBlock(int x, int y, int z) {
    //center
    if (x >= 0 && x < 16 && z >= 0 && z < 16) {
        return center->getBlock(x, y, z);
    }

    //positive X
    if (x >= 16 && z >= 0 && z < 16) {
        return posX->getBlock(x - 16, y, z);
    }
    //negative X
    if (x < 0 && z >= 0 && z < 16) {
        return negX->getBlock(x + 16, y, z);
    }

    //positive Z
    if (x >= 0 && x < 16 && z >= 16) {
        return posZ->getBlock(x, y, z - 16);
    }
    //negative Z
    if (x >= 0 && x < 16 && z < 0) {
        return negZ->getBlock(x, y, z + 16);
    }

    //positive X positive Z
    if (x >= 16 &&  z >= 16) {
        return posXposZ->getBlock(x - 16, y, z - 16);
    }
    //positive X negative Z
    if (x >= 16 && z < 0) {
        return posXnegZ->getBlock(x - 16, y, z + 16);
    }

    //negative X positive Z
    if (x < 0 &&  z >= 16) {
        return negXposZ->getBlock(x + 16, y, z - 16);
    }
    //negative X negative Z
    if (x < 0 && z < 0) {
        return negXnegZ->getBlock(x + 16, y, z + 16);
    }

    return 0;
}

void ChunkNeighborhood::setBlock(int x, int y, int z, char block) {

}

int ChunkNeighborhood::getSunlight(int x, int y, int z) {
    //center
    if (x >= 0 && x < 16 && z >= 0 && z < 16) {
        return center->getSunlight(x, y, z);
    }

    //positive X
    if (x >= 16 && z >= 0 && z < 16) {
        return posX->getSunlight(x - 16, y, z);
    }
    //negative X
    if (x < 0 && z >= 0 && z < 16) {
        return negX->getSunlight(x + 16, y, z);
    }

    //positive Z
    if (x >= 0 && x < 16 && z >= 16) {
        return posZ->getSunlight(x, y, z - 16);
    }
    //negative Z
    if (x >= 0 && x < 16 && z < 0) {
        return negZ->getSunlight(x, y, z + 16);
    }

    //positive X positive Z
    if (x >= 16 &&  z >= 16) {
        return posXposZ->getSunlight(x - 16, y, z - 16);
    }
    //positive X negative Z
    if (x >= 16 && z < 0) {
        return posXnegZ->getSunlight(x - 16, y, z + 16);
    }

    //negative X positive Z
    if (x < 0 &&  z >= 16) {
        return negXposZ->getSunlight(x + 16, y, z - 16);
    }
    //negative X negative Z
    if (x < 0 && z < 0) {
        return negXnegZ->getSunlight(x + 16, y, z + 16);
    }

    return 15;
}

void ChunkNeighborhood::setSunlight(int x, int y, int z, int val) {
    //center
    if (x >= 0 && x < 16 && z >= 0 && z < 16) {
        center->setSunlight(x, y, z, val);
    }

    //positive X
    if (x >= 16 && z >= 0 && z < 16) {
        posX->setSunlight(x - 16, y, z, val);
    }
    //negative X
    if (x < 0 && z >= 0 && z < 16) {
        negX->setSunlight(x + 16, y, z, val);
    }

    //positive Z
    if (x >= 0 && x < 16 && z >= 16) {
        posZ->setSunlight(x, y, z - 16, val);
    }
    //negative Z
    if (x >= 0 && x < 16 && z < 0) {
        negZ->setSunlight(x, y, z + 16, val);
    }

    //positive X positive Z
    if (x >= 16 &&  z >= 16) {
        posXposZ->setSunlight(x - 16, y, z - 16, val);
    }
    //positive X negative Z
    if (x >= 16 && z < 0) {
        posXnegZ->setSunlight(x - 16, y, z + 16, val);
    }

    //negative X positive Z
    if (x < 0 &&  z >= 16) {
        negXposZ->setSunlight(x + 16, y, z - 16, val);
    }
    //negative X negative Z
    if (x < 0 && z < 0) {
        negXnegZ->setSunlight(x + 16, y, z + 16, val);
    }
}

int ChunkNeighborhood::getTorchlight(int x, int y, int z) {
    //center
    if (x >= 0 && x < 16 && z >= 0 && z < 16) {
        return center->getTorchlight(x, y, z);
    }

    //positive X
    if (x >= 16 && z >= 0 && z < 16) {
        return posX->getTorchlight(x - 16, y, z);
    }
    //negative X
    if (x < 0 && z >= 0 && z < 16) {
        return negX->getTorchlight(x + 16, y, z);
    }

    //positive Z
    if (x >= 0 && x < 16 && z >= 16) {
        return posZ->getTorchlight(x, y, z - 16);
    }
    //negative Z
    if (x >= 0 && x < 16 && z < 0) {
        return negZ->getTorchlight(x, y, z + 16);
    }

    //positive X positive Z
    if (x >= 16 &&  z >= 16) {
        return posXposZ->getTorchlight(x - 16, y, z - 16);
    }
    //positive X negative Z
    if (x >= 16 && z < 0) {
        return posXnegZ->getTorchlight(x - 16, y, z + 16);
    }

    //negative X positive Z
    if (x < 0 &&  z >= 16) {
        return negXposZ->getTorchlight(x + 16, y, z - 16);
    }
    //negative X negative Z
    if (x < 0 && z < 0) {
        return negXnegZ->getTorchlight(x + 16, y, z + 16);
    }

    return 0;
}

void ChunkNeighborhood::setTorchlight(int x, int y, int z, int val) {
    //center
    if (x >= 0 && x < 16 && z >= 0 && z < 16) {
        center->setTorchlight(x, y, z, val);
    }

    //positive X
    if (x >= 16 && z >= 0 && z < 16) {
        posX->setTorchlight(x - 16, y, z, val);
    }
    //negative X
    if (x < 0 && z >= 0 && z < 16) {
        negX->setTorchlight(x + 16, y, z, val);
    }

    //positive Z
    if (x >= 0 && x < 16 && z >= 16) {
        posZ->setTorchlight(x, y, z - 16, val);
    }
    //negative Z
    if (x >= 0 && x < 16 && z < 0) {
        negZ->setTorchlight(x, y, z + 16, val);
    }

    //positive X positive Z
    if (x >= 16 &&  z >= 16) {
        posXposZ->setTorchlight(x - 16, y, z - 16, val);
    }
    //positive X negative Z
    if (x >= 16 && z < 0) {
        posXnegZ->setTorchlight(x - 16, y, z + 16, val);
    }

    //negative X positive Z
    if (x < 0 &&  z >= 16) {
        negXposZ->setTorchlight(x + 16, y, z - 16, val);
    }
    //negative X negative Z
    if (x < 0 && z < 0) {
        negXnegZ->setTorchlight(x + 16, y, z + 16, val);
    }
}



World::World() {

}

void World::init(Context ctx) {
    this->ctx = ctx;
}

void World::update(float delta) {
}

int World::getBlock(int x, int y, int z) {
    int xp = x >> 4;
    int zp = z >> 4;

    std::shared_ptr<Chunk> c = getChunk(xp, zp);

    return c->getBlock(x & 15, y, z & 15);
}

void World::setBlock(int x, int y, int z, int block) {
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
    }

    return chunks[{x, z}];
}

ChunkNeighborhood World::getChunkNeighborhood(int x, int z) {
    ChunkNeighborhood neighborhood = {
            getChunk(x, z),

            getChunk(x+1, z),
            getChunk(x-1, z),

            getChunk(x, z+1),
            getChunk(x, z-1),

            getChunk(x+1, z+1),
            getChunk(x+1, z-1),

            getChunk(x-1, z+1),
            getChunk(x-1, z-1)
    };

    return neighborhood;
}

void World::deleteChunk(int x, int z) {
    if (chunkExists(x, z)) {
        //std::cout << "deleting chunk" << std::endl;
        chunks.erase({x, z});
    }
}

void World::shutdown() {
    chunks.clear();
}

bool World::chunkExists(int x, int z) {
    bool exists = chunks.find({x, z}) != chunks.end();

    return exists;
}

std::unordered_map<vec2i, std::shared_ptr<Chunk>> World::getChunks() {
    //chunks_mx.lock();
    std::unordered_map<vec2i, std::shared_ptr<Chunk>> ret = chunks;
    //chunks_mx.unlock();

    return ret;
}

bool World::raycastBlocks(vec3 origin, vec3 direction, float maxDistance, vec3i &blockPosReturn, vec3 &blockNormalReturn) {
    std::shared_ptr<Chunk> nearestBlockChunk;
    vec3i nearestBlockPos;
    float nearestBlockDistance = 10000000000000.0f;
    vec3 nearestBlockNormal;

    for (auto const &ref: chunks) {
        std::shared_ptr<Chunk> c = ref.second;

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

    if (nearestBlockDistance != 10000000000000.0f) {
        vec3i chunkPos = vec3i(nearestBlockChunk->chunk_x * 16, 0, nearestBlockChunk->chunk_z * 16);

        blockNormalReturn = nearestBlockNormal;
        blockPosReturn = nearestBlockPos + chunkPos;
        return true;
    }
    return false;
}

std::vector<AABB> World::getCollisions(AABB test) {
    vec3i start = vec3i(test.min.x, test.min.y, test.min.z);
    vec3i end = vec3i(test.max.x, test.max.y, test.max.z);

    std::vector<AABB> returnVector;

    for (int x = start.x; x <= end.x; x++) {
        for (int y = start.y; y <= end.y; y++) {
            for (int z = start.z; z <= end.z; z++) {
                int xp = x >> 4;
                int zp = z >> 4;

                if (chunkExists(xp, zp)) {
                    if (getBlock(x, y, z) != 0) {
                        AABB blockAbb = AABB(vec3(x, y, z), vec3(x+1, y+1, z+1));

                        returnVector.push_back(blockAbb);
                    }
                }
            }
        }
    }

    return returnVector;
}