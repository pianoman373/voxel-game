#include "World.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Settings.hpp"
#include "Util.hpp"
#include "Client.hpp"
#include "Chunk.hpp"
#include "sol.hpp"
#include "Common.hpp"

#include <crucible/Renderer.hpp>
#include <crucible/Input.hpp>
#include <crucible/Camera.hpp>
#include <crucible/AABB.hpp>
#include <crucible/Frustum.hpp>

#include <iostream>
#include <mutex>
#include <thread>

static std::vector<chunk_position> chunkRequests;
static std::mutex chunkRequestsMutex;


static void placeBlocks(Chunk *chunk) {
    sol::table t = Common::world.luaState["generateChunk"](chunk->chunk_x, chunk->chunk_y, chunk->chunk_z);


    for (int x = 1; x <= CHUNK_SIZE; x++) {
        for (int y = 1; y <= CHUNK_SIZE; y++) {
            for (int z = 1; z <= CHUNK_SIZE; z++) {
                //int block = t[x + 32*(y-1) + 32*32*(z-1)];
                int block = t[x][y][z];

                if (block != 0) {
                    chunk->setBlock(x-1, y-1, z-1, block);
                    //chunk->blocks[x-1][y-1][z-1] = (char)block;
                }
            }
        }
    }
    chunk->empty = false;
    chunk->rebuild = true;
}

World::~World() {
    for (auto const &ref: chunks) {
        delete ref.second;
    }
}

World::World() {
    std::cout << "creating world instance" << std::endl;
    luaState.open_libraries(sol::lib::base,
                            sol::lib::bit32,
                            sol::lib::coroutine,
                            sol::lib::count,
                            sol::lib::io,
                            sol::lib::math,
                            sol::lib::os,
                            sol::lib::package,
                            sol::lib::string,
                            sol::lib::table,
                            sol::lib::utf8,
                            sol::lib::ffi
                        );
    luaState.script_file("api.lua");
    luaState.script_file("worldgen.lua");

    int range = 1;
    for (int i = -range; i < range; i++) {
        for (int j = -range; j < range; j++) {
            for (int k = -range; k < range; k++) {
                chunkLoadingPositions.push_back(vec3i(i, j, k));
            }
        }
    }


    std::sort(chunkLoadingPositions.begin(), chunkLoadingPositions.end());
}

void World::addChunk(int x, int y, int z, Chunk *c) {
    chunk_position pos = {x, y, z};
    chunks.insert(std::make_pair(pos, c));
}

void World::generateNewChunk(int x, int y, int z) {
    Chunk *c = new Chunk(x, y, z, &Common::world);

    placeBlocks(c);

    Common::world.addChunk(x, y, z, c);

    notifyChunkChange(x, y, z);
}

void World::deleteChunk(int x, int y, int z) {
    chunk_position pos = {x, y, z};
    Chunk *c = chunks.at(pos);
    chunks.erase(pos);
    delete c;
}

void World::rebuild() {
    chunksToUpdateMutex.lock();
    std::vector<Chunk*> localChunks = chunksToUpdate;
    chunksToUpdate.clear();
    chunksToUpdateMutex.unlock();

    for (int i = 0; i < localChunks.size(); i++) {
        localChunks[i]->generateMesh();
    }

//    for (auto const ref: chunks) {
//        Chunk *c = ref.second;
//
//        chunk_position pos = ref.first;
//
//        if (c->rebuild) {
//            c->generateMesh();
//        }
//    }
}

void World::render(Camera &cam, Material *nearmaterial, Material *farmaterial) {
    for (auto const &ref: chunks) {
        Chunk *c = ref.second;

        if (!c->empty || true) {

            if (c->rebuild) {
                c->ms.toMesh(c->mesh);
                c->rebuild = false;
            }


            vec3 chunkPos = vec3(c->chunk_x * CHUNK_SIZE, (c->chunk_y * CHUNK_SIZE), c->chunk_z * CHUNK_SIZE);
            Renderer::renderDebugAABB(AABB(chunkPos , chunkPos + vec3(CHUNK_SIZE)), vec3(1.0f, 0.0f, 0.0f));


            vec3 chunkCenterPos = chunkPos + vec3(CHUNK_SIZE / 2.0f);

            //we use square distance because computing square roots would be an extra step and hurt performance
            float squareDistanceToChunk = lengthSquared(cam.getPosition() - chunkCenterPos);

            if (true || Client::frustum.isBoxInside(AABB(chunkPos , chunkPos + vec3(CHUNK_SIZE))) || AABB(chunkPos , chunkPos + vec3(CHUNK_SIZE)).isVecInside(cam.getPosition())) {
                //it is much faster to square our render distance rather than square rooting our chunk distance
                if (squareDistanceToChunk < (128.0f) * (128.0f) && squareDistanceToChunk < (Settings::render_distance * Settings::render_distance)) {
                    Renderer::render(&c->mesh, nearmaterial, Transform(chunkPos, vec3(), vec3(1.0f)), AABB(chunkPos , chunkPos + vec3(CHUNK_SIZE)));
                }
                else if (squareDistanceToChunk < (Settings::render_distance * Settings::render_distance)) {
                    Renderer::render(&c->mesh, farmaterial, Transform(chunkPos, vec3(), vec3(1.0f)), AABB(chunkPos , chunkPos + vec3(CHUNK_SIZE)));
                }
            }
        }
    }
}

int World::getBlock(int x, int y, int z) {
    int xp = x >> 5;
    int yp = y >> 5;
    int zp = z >> 5;

    chunk_position pos = {xp, yp, zp};

    if (chunkExists(xp, yp, zp)) {
        Chunk *c = chunks.at(pos);

        return c->getBlock(x & 31, y & 31, z & 31);
    }

    return 1;
}

Chunk *World::getChunk(int x, int y, int z) {
    return chunks.at({x, y, z});
}

bool World::chunkExists(int x, int y, int z) {
    return chunks.find({x, y, z}) != chunks.end();
}

void World::setBlock(int x, int y, int z, int block) {
    int xp = x >> 5;
    int yp = y >> 5;
    int zp = z >> 5;

    if (chunkExists(xp, yp, zp)) {
        chunk_position pos = {xp, yp, zp};

        Chunk *c = chunks[pos];

        c->setBlock(x & 31, y & 31, z & 31, block);

        notifyChunkChange(pos.x, pos.y, pos.z);
    }
}

void World::notifyChunkChange(int x, int y, int z) {
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            for (int k = -1; k <= 1; k++) {
                if (chunkExists(x + i, y + j, z + k)) {
                    getChunk(x + i, y + j, z + k)->rebuild = true;

                    chunksToUpdateMutex.lock();
                    chunksToUpdate.push_back(getChunk(x + i, y + j, z + k));
                    chunksToUpdateMutex.unlock();
                }
            }
        }
    }
}

void World::update(const Camera &cam, float delta) {
    vec3i chunkPos = World::worldToChunkPos(cam.position);

    for (int i = 0; i < chunkLoadingPositions.size(); i++) {
        vec3i position = chunkLoadingPositions[i];

        if (!chunkExists(position.x + chunkPos.x, position.y + chunkPos.y, position.z + chunkPos.z)) {
            generateNewChunk(position.x + chunkPos.x, position.y + chunkPos.y, position.z + chunkPos.z);

        }
    }
}

bool World::raycastBlocks(vec3 origin, vec3 direction, float maxDistance, vec3i &blockPosReturn, vec3 &blockNormalReturn) {
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

std::vector<AABB> World::getCollisions(AABB test) {
    std::vector<AABB> returnVector;

    for (auto const &ref: chunks) {
        Chunk *c = ref.second;

        if (!c->empty) {
            vec3 chunkPos = vec3(c->chunk_x * CHUNK_SIZE, (c->chunk_y * CHUNK_SIZE), c->chunk_z * CHUNK_SIZE);

            if (test.intersectsWith(AABB(chunkPos, chunkPos + vec3(CHUNK_SIZE)))) {

                for (int x = 0; x < CHUNK_SIZE; x++) {
                    for (int y = 0; y < CHUNK_SIZE; y++) {
                        for (int z = 0; z < CHUNK_SIZE; z++) {
                            char block = c->getBlock(x, y, z);
                            if (block != 0) {
                                AABB blockAbb = AABB(chunkPos + vec3(x, y, z), chunkPos + vec3(x + 1, y + 1, z + 1));

                                if (blockAbb.intersectsWith(test)) {
                                    returnVector.push_back(blockAbb);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return returnVector;
}

vec3i World::worldToChunkPos(vec3 pos) {
    return vec3i((int)pos.x >> 5, (int)pos.y >> 5, (int)pos.z >> 5);
}