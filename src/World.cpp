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

static float heightAt(vec3 pos) {
    return Util::ridgedNoise(vec2(pos.x/2.0f, pos.z/2.0f), 5, 0.003f, 0.5f) * 50.0f + 70.0f;
}

static void placeBlocks(Chunk *chunk) {
    sol::table t = Common::world.luaState["generateChunk"](chunk->chunk_x, chunk->chunk_y, chunk->chunk_z);


    for (int x = 1; x <= CHUNK_SIZE; x++) {
        for (int y = 1; y <= CHUNK_SIZE; y++) {
            for (int z = 1; z <= CHUNK_SIZE; z++) {
                //int block = t[x + 32*(y-1) + 32*32*(z-1)];
                int block = t[x][y][z];

                if (block != 0) {
                    chunk->setBlock(x-1, y-1, z-1, block);
                }
            }
        }
    }
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
}

void World::generate(bool empty) {

}

void World::addChunk(int x, int y, int z, Chunk *c) {
    chunk_position pos = {x, y, z};
    chunks.insert(std::make_pair(pos, c));
}

void World::generateNewChunk(int x, int y, int z) {
    Chunk *c = new Chunk(x, y, z, this);

    placeBlocks(c);
    addChunk(x, y, z, c);
}

void World::deleteChunk(int x, int y, int z) {
    chunk_position pos = {x, y, z};
    Chunk *c = chunks.at(pos);
    delete c;
}

void World::rebuild() {
    int i = 0;
    for (auto const ref: chunks) {
        Chunk *c = ref.second;

        chunk_position pos = ref.first;

        for (int x = pos.x - 1; x < (pos.x + 2); x++) {
            for (int y = pos.y - 1; y < (pos.y + 2); y++) {
                for (int z = pos.z - 1; z < (pos.z + 2); z++) {
                    if (!chunkExists(x, y, z)) {
                        goto end;
                    }
                }
            }
        }


        if (c->rebuild) {
            c->generateMesh();
            i++;
            if (i > 10)
                return;
        }

        end:
        ;
    }
}

void World::render(Camera &cam, Material *nearmaterial, Material *farmaterial) {
    for (auto const &ref: chunks) {
        Chunk *c = ref.second;

        if (!c->empty) {


            vec3 chunkPos = vec3(c->chunk_x * CHUNK_SIZE, (c->chunk_y * CHUNK_SIZE), c->chunk_z * CHUNK_SIZE);
            //Renderer::renderDebugAABB(AABB(chunkPos , chunkPos + vec3(CHUNK_SIZE)), vec3(1.0f, 0.0f, 0.0f));


            vec3 chunkCenterPos = chunkPos + vec3(CHUNK_SIZE / 2.0f);

            //we use square distance because computing square roots would be an extra step and hurt performance
            float squareDistanceToChunk = lengthSquared(cam.getPosition() - chunkCenterPos);

            if (Client::frustum.isBoxInside(AABB(chunkPos , chunkPos + vec3(CHUNK_SIZE))) || AABB(chunkPos , chunkPos + vec3(CHUNK_SIZE)).isVecInside(cam.getPosition())) {
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

    if (x > 0 && y > 0 && z > 0) {

        chunk_position pos = {x / CHUNK_SIZE, y / CHUNK_SIZE, z / CHUNK_SIZE};

        Chunk *c;

        try {
            c = chunks.at(pos);
        }
        catch (std::out_of_range exception) {
            //std::cout << "caught exception" << std::endl;
            return 0;
        }

        return c->getBlock(x % CHUNK_SIZE, y % CHUNK_SIZE, z % CHUNK_SIZE);
    }
    return 1;
}

Chunk *World::getChunk(int x, int y, int z) {
    return chunks.at({x, y, z});
}

bool World::chunkExists(int x, int y, int z) {
    try {
        chunks.at({x, y, z});
        return true;
    }
    catch (std::out_of_range exception) {
        return false;
    }
}

void World::setBlock(int x, int y, int z, int block) {
    chunk_position pos = {x / CHUNK_SIZE, y / CHUNK_SIZE, z / CHUNK_SIZE};

    Chunk *c;

    try {
        c = chunks[pos];
    }
    catch (std::out_of_range exception) {
		return;
        //std::cout << "caught exception" << std::endl;
    }

    vec3i bpos = vec3i(x % CHUNK_SIZE, y % CHUNK_SIZE, z % CHUNK_SIZE);

    c->setBlock(bpos.x, bpos.y, bpos.z, block);
    //c->rebuild = true;

    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            for (int k = -1; k < 2; k++) {
                if (chunkExists(pos.x + i, pos.y + j, pos.z + k)) {
                    getChunk(pos.x + i, pos.y + j, pos.z + k)->rebuild = true;
                }
            }
        }
    }
}

void World::update(Camera &cam, float delta) {

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
