#include "World.hpp"
#include <iostream>
#include "GL/glew.h"
#include "Renderer.hpp"
#include "AABB.hpp"
#include "Input.hpp"
#include "GLFW/glfw3.h"
#include "Settings.hpp"

World::~World() {
    for (auto const &ref: chunks) {
        delete ref.second;
    }
}

World::World(): player(*this) {

}

void World::addChunk(int x, int y, int z, Chunk *c) {
    chunk_position pos = {x, y, z};
    chunks.insert(std::make_pair(pos, c));
}

void World::deleteChunk(int x, int y, int z) {
    chunk_position pos = {x, y, z};
    Chunk *c = chunks.at(pos);
    delete c;
}

void World::rebuild() {
    for (auto const &ref: chunks) {
        Chunk *c = ref.second;

        if (c->rebuild) {
            c->generateMesh();
            c->rebuild = false;
        }
    }
}

void World::render(Camera &cam, Shader shader, Texture tex) {
    for (auto const &ref: chunks) {
        Chunk *c = ref.second;

        if (!c->empty) {
            Material mat;
            mat.setShader(shader);
            mat.setUniformTexture("tex4", tex, 4);

            vec3 chunkPos = vec3(c->chunk_x * CHUNK_SIZE, (c->chunk_y * CHUNK_SIZE), c->chunk_z * CHUNK_SIZE);

            if (lengthSquared(cam.getPosition() - (chunkPos + (CHUNK_SIZE / 2.0f))) < (Settings::render_distance * Settings::render_distance)) {
                Renderer::render(&c->mesh, mat, Transform(chunkPos, vec3(), vec3()));
            }
        }
    }
}

int World::getBlock(int x, int y, int z) {
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

void World::setBlock(int x, int y, int z, int block) {
    chunk_position pos = {x / CHUNK_SIZE, y / CHUNK_SIZE, z / CHUNK_SIZE};

    Chunk *c;

    try {
        c = chunks.at(pos);
    }
    catch (std::out_of_range exception) {
        //std::cout << "caught exception" << std::endl;
    }

    c->setBlock(x % CHUNK_SIZE, y % CHUNK_SIZE, z % CHUNK_SIZE, block);
    c->rebuild = true;
}

void World::update(Camera &cam, float delta) {
    player.update(cam, delta);
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
        //vec3 bias = vec3(0.001f);
        //vec3 blockPos = vec3(nearestBlockPos.x, nearestBlockPos.y, nearestBlockPos.z);
        //Renderer::renderDebugAABB(chunkPos + blockPos - bias, chunkPos + blockPos + 1.0f + bias, vec3());

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
