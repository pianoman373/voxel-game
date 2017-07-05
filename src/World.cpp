#include "World.hpp"
#include <iostream>
#include "GL/glew.h"
#include "Renderer.hpp"
#include "AABB.hpp"
#include "Input.hpp"
#include "GLFW/glfw3.h"
#include "Settings.hpp"
#include "Util.hpp"

static float heightAt(vec3 pos) {
    return Util::ridgedNoise(vec2(pos.x, pos.z), 5, 0.003f, 0.5f) * 30.0f + 20.0f;
}

static void placeBlocks(Chunk *chunk) {
    //std::cout << "generating chunk" << std::endl;

    //double time1 = glfwGetTime();
    for (int x = 0; x < CHUNK_SIZE; x++) {
        //for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                float height = heightAt(vec3(x + (chunk->chunk_x * CHUNK_SIZE), 0.0f, z + (chunk->chunk_z * CHUNK_SIZE)));

                //create vectors which begin at this block and extend to the next block over
                vec3 lineNegX =  normalize(vec3(x - 1, heightAt(vec3(x + (chunk->chunk_x * CHUNK_SIZE) - 1, 0.0f, z + (chunk->chunk_z * CHUNK_SIZE))), z) - vec3(x, height, z));
                vec3 linePosX =  normalize(vec3(x + 1, heightAt(vec3(x + (chunk->chunk_x * CHUNK_SIZE) + 1, 0.0f, z + (chunk->chunk_z * CHUNK_SIZE))), z) - vec3(x, height, z));

                vec3 lineNegY =  normalize(vec3(x, heightAt(vec3(x + (chunk->chunk_x * CHUNK_SIZE), 0.0f, z + (chunk->chunk_z * CHUNK_SIZE) - 1)), z - 1) - vec3(x, height, z));
                vec3 linePosY =  normalize(vec3(x, heightAt(vec3(x + (chunk->chunk_x * CHUNK_SIZE), 0.0f, z + (chunk->chunk_z * CHUNK_SIZE) + 1)), z + 1) - vec3(x, height, z));

                //cross those vectors to get upwards pointing lines
                vec3 norm1 = cross(lineNegY, lineNegX);
                vec3 norm2 = cross(linePosX, lineNegY);

                vec3 norm3 = cross(linePosY, linePosX);
                vec3 norm4 = cross(lineNegX, linePosY);


                //average them together to get what is effectively a surface normal if this terrain were to be smoothed out
                vec3 finalNorm = (norm1 + norm2 + norm3 + norm4) / 4.0f;


                //std::cout << lineNegX << std::endl;

                height -= (chunk->chunk_y * 32);
                for (int i = 0; i < height; i++) {
                    if (dot(finalNorm, vec3(0.0f, 1.0f, 0.0f)) < 0.7f) {
                        chunk->setBlock(x, i, z, 2);
                    }
                    else {
                        chunk->setBlock(x, i, z, 3);
                    }
                }
                if (dot(finalNorm, vec3(0.0f, 1.0f, 0.0f)) < 0.7f) {
                    chunk->setBlock(x, floor(height), z, 2);
                }
                else {
                    chunk->setBlock(x, floor(height), z, 1);
                }
            }
        //}
    }
    //std::cout << "generated chunk in: " << glfwGetTime() - time1 << std::endl;
    //chunk->generateMesh();
}

World::~World() {
    for (auto const &ref: chunks) {
        delete ref.second;
    }
}

World::World() {
    //std::cout << "creating world instance" << std::endl;
}

void World::generate(bool empty) {
    for (int x = 0; x < WORLD_SIZE; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            for (int z = 0; z < WORLD_SIZE; z++) {
                Chunk *c = new Chunk(x, y, z, this);
                if (!empty)
                    placeBlocks(c);
                addChunk(x, y, z, c);
            }
        }
    }
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
    for (auto const ref: chunks) {
        Chunk *c = ref.second;

        if (c->rebuild) {
            c->generateMesh();
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

    c->setBlock(x % CHUNK_SIZE, y % CHUNK_SIZE, z % CHUNK_SIZE, block);
    c->rebuild = true;
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
