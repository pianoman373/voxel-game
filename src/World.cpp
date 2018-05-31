#include "World.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Settings.hpp"
#include "Util.hpp"
#include "Client.hpp"
#include "Chunk.hpp"
#include "sol.hpp"
#include "SimplexNoise.hpp"

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

const int renderdistance = 6;

static float getHeight(int x, int z) {
	return Util::ridgedNoise(vec2(x / 10.0f, z / 10.0f), 5, 0.01f, 0.5f) * 20.0f + 0.0f;
}

char topBlock = 1;    //grass
char midBlock = 3;    //dirt
char fillerBlock = 2; //stone

static void generateChunk(Chunk *chunk) {
	float heights[34][34]; //heightmap includes adjacent chunk blocks

	for (int x = 0; x < 34; x++) {
		for (int z = 0; z < 34; z++) {
			heights[x][z] = getHeight(x - 1 + (chunk->chunk_x * 32), z - 1 + (chunk->chunk_z * 32));
		}
	}

	for (int x = 0; x < 32; x++) {
		for (int z = 0; z < 32; z++) {
			float height = heights[x + 1][z + 1];

			vec3 lineNegX = normalize(vec3(x - 1, heights[x + 2][z + 1], z) - vec3(x, height, z));
			vec3 linePosX = normalize(vec3(x + 1, heights[x][z + 1], z) - vec3(x, height, z));

			vec3 lineNegY = normalize(vec3(x, heights[x + 1][z], z - 1) - vec3(x, height, z));
			vec3 linePosY = normalize(vec3(x, heights[x + 1][z + 2], z + 1) - vec3(x, height, z));

			vec3 norm1 = cross(lineNegY, lineNegX);
			vec3 norm2 = cross(linePosX, lineNegY);

			vec3 norm3 = cross(linePosY, linePosX);
			vec3 norm4 = cross(lineNegX, linePosY);

			vec3 finalNorm = (norm1 + norm2 + norm3 + norm4) / vec3(4, 4, 4);
			bool steep = dot(finalNorm, vec3(0, 1, 0)) < 0.7f;

			for (int y = 0; y < 32; y++) {
                int actualX = x - 1 + (chunk->chunk_x * 32);
                int actualY = y - 1 + (chunk->chunk_y * 32);
                int actualZ = z - 1 + (chunk->chunk_z * 32);

                if (SimplexNoise::noise((float)actualX / 30.0f, (float)actualY / 30.0f, (float)actualZ / 30.0f) > -0.5f) {

					int actualHeight = y + (chunk->chunk_y * 32);

					if (actualHeight < height) {
						if (steep) {
							chunk->setBlock(x, y, z, fillerBlock);
						}
						if (actualHeight < height - 5) {
							chunk->setBlock(x, y, z, fillerBlock);
						} else if (actualHeight < height - 1) {
							chunk->setBlock(x, y, z, midBlock);
						} else {
							chunk->setBlock(x, y, z, topBlock);
						}
					}
				}
			}
		}
	}
}

static void placeBlocks(Chunk *chunk) {
	generateChunk(chunk);

    chunk->empty = false;
}


void World::shutdown() {
	running = false;
	updateThread.join();

    for (auto const &ref: chunks) {
        delete ref.second;
    }
}

void World::init(const Camera& camera) {
	int range = renderdistance - 1;
	for (int i = -range; i <= range; i++) {
		for (int j = -range; j <= range; j++) {
			for (int k = -range; k <= range; k++) {
				chunkLoadingPositions.push_back(vec3i(i, j, k));
			}
		}
	}

	std::sort(chunkLoadingPositions.begin(), chunkLoadingPositions.end());

	updateThread = std::thread(&World::loadChunks, this, camera);
}

void World::deleteChunk(int x, int y, int z) {
    chunk_position pos = {x, y, z};
    Chunk *c = chunks.at(pos);
    chunks.erase(pos);
    delete c;
}

void World::rebuild() {
	
}

void World::render(Camera &cam, Material *nearmaterial, Material *farmaterial) {
	vec3i camPos = World::worldToChunkPos(cam.position);
	bool rebuild = true;

	auto chunksCopy = chunks;

    for (auto const &ref: chunksCopy) {
        Chunk *c = ref.second;
        chunk_position chunkPos = ref.first;

        if (c->isDirty) {

            chunksToRebuildMutex.lock();
            chunksToRebuild.push_back(c);
            chunksToRebuildMutex.unlock();
            c->isDirty = false;
        }
        

		if (abs(chunkPos.x - camPos.x) > renderdistance || abs(chunkPos.y - camPos.y) > renderdistance || abs(chunkPos.z - camPos.z) > renderdistance) {
			chunksToDelete.push_back(chunkPos);
		}
		else if (!c->empty) {
            vec3 chunkPos = vec3(c->chunk_x * CHUNK_SIZE, (c->chunk_y * CHUNK_SIZE), c->chunk_z * CHUNK_SIZE);
            Renderer::debug.renderDebugAABB(AABB(chunkPos, chunkPos + vec3(CHUNK_SIZE)), vec3(1.0f, 0.0f, 0.0f));


            vec3 chunkCenterPos = chunkPos + vec3(CHUNK_SIZE / 2.0f);

            //we use square distance because computing square roots would be an extra step and hurt performance
            float squareDistanceToChunk = lengthSquared(cam.getPosition() - chunkCenterPos);

            if (true || Client::frustum.isBoxInside(AABB(chunkPos, chunkPos + vec3(CHUNK_SIZE))) || AABB(chunkPos, chunkPos + vec3(CHUNK_SIZE)).isVecInside(cam.getPosition())) {
                //it is much faster to square our render distance rather than square rooting our chunk distance
                if (squareDistanceToChunk < (128.0f) * (128.0f) && squareDistanceToChunk < (Settings::render_distance * Settings::render_distance)) {
					c->render(nearmaterial);
                } else if (squareDistanceToChunk < (Settings::render_distance * Settings::render_distance)) {
					c->render(farmaterial);
                }
            }
        }
    }


	/*chunksMutex.lock();

	for (chunk_position i : chunksToDelete) {
		deleteChunk(i.x, i.y, i.z);
	}
	chunksToDelete.clear();

	chunksMutex.unlock();*/

	if (!chunksToDelete.empty()) {
		chunk_position cpos = chunksToDelete.back();
		chunksMutex.lock();
		if (chunkExists(cpos.x, cpos.y, cpos.z)) {
			deleteChunk(cpos.x, cpos.y, cpos.z);
		}
		chunksMutex.unlock();
		chunksToDelete.pop_back();
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
	if (chunkExists(x, y, z)) {
		return chunks.at({ x, y, z });
	}
	else {
		Chunk *c = new Chunk(x, y, z, this);

		chunk_position pos = { x, y, z };
		chunks.insert(std::make_pair(pos, c));

		return c;
	}
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

		notifyChunkChange(x, y, z);
    }
}

void World::notifyChunkChange(int x, int y, int z) {
	int xp = x >> 5;
	int yp = y >> 5;
	int zp = z >> 5;

	if (chunkExists(xp, yp, zp)) {
		chunk_position pos = { xp, yp, zp };

		Chunk *c = chunks[pos];

		chunksToRebuildMutex.lock();
		chunksToRebuild.push_front(c);
		chunksToRebuildMutex.unlock();

		int xc = x & 31;
		int yc = y & 31;
		int zc = z & 31;

		if (xc == 31 && chunkExists(xp + 1, yp, zp)) {
			pos = { xp + 1, yp, zp };

			c = chunks[pos];

			chunksToRebuildMutex.lock();
			chunksToRebuild.push_front(c);
			chunksToRebuildMutex.unlock();
		}
		if (xc == 0 && chunkExists(xp - 1, yp, zp)) {
			pos = { xp - 1, yp, zp };

			c = chunks[pos];

			chunksToRebuildMutex.lock();
			chunksToRebuild.push_front(c);
			chunksToRebuildMutex.unlock();
		}

		if (yc == 31 && chunkExists(xp, yp + 1, zp)) {
			pos = { xp, yp + 1, zp };

			c = chunks[pos];

			chunksToRebuildMutex.lock();
			chunksToRebuild.push_front(c);
			chunksToRebuildMutex.unlock();
		}
		if (yc == 0 && chunkExists(xp, yp - 1, zp)) {
			pos = { xp, yp - 1, zp };

			c = chunks[pos];

			chunksToRebuildMutex.lock();
			chunksToRebuild.push_front(c);
			chunksToRebuildMutex.unlock();
		}

		if (zc == 31 && chunkExists(xp, yp, zp + 1)) {
			pos = { xp, yp, zp + 1 };

			c = chunks[pos];

			chunksToRebuildMutex.lock();
			chunksToRebuild.push_front(c);
			chunksToRebuildMutex.unlock();
		}
		if (zc == 0 && chunkExists(xp, yp, zp - 1)) {
			pos = { xp, yp, zp -1};

			c = chunks[pos];

			chunksToRebuildMutex.lock();
			chunksToRebuild.push_front(c);
			chunksToRebuildMutex.unlock();
		}
	}
}

void World::update(const Camera &cam, float delta) {
	time += delta;

	if (time > DAYLIGHT_CYCLE) {
		time -= DAYLIGHT_CYCLE;
	}
}

void World::loadChunks(const Camera &cam) {
	while (running) {
		
		if (!chunksToRebuild.empty()) {
			chunksToRebuildMutex.lock();
			Chunk* c = chunksToRebuild.front();
			chunksToRebuild.pop_front();
			chunksToRebuildMutex.unlock();

			c->generateMesh();
		}

		for (vec3i i : chunkLoadingPositions) {
			vec3i actualPosition = i + World::worldToChunkPos(cam.position);

			if (!chunkExists(actualPosition.x, actualPosition.y, actualPosition.z) || !getChunk(actualPosition.x, actualPosition.y, actualPosition.z)->isMeshGenerated) {


				for (int x = -1; x < 2; x++) {
					for (int y = -1; y < 2; y++) {
						for (int z = -1; z < 2; z++) {
							if (!chunkExists(actualPosition.x + x, actualPosition.y + y, actualPosition.z + z)) {
								chunksMutex.lock();
								placeBlocks(getChunk(actualPosition.x + x, actualPosition.y + y, actualPosition.z + z));
								chunksMutex.unlock();
							}
						}
					}
				}


				getChunk(actualPosition.x, actualPosition.y, actualPosition.z)->generateMesh();

				break;
			}
		}
	}
}

float World::getTime() {
	return this->time;
}

void World::setTime(float time) {
	this->time = time;
}

vec3 World::getSunDirection() {
	float rotation = ((getTime() / DAYLIGHT_CYCLE) * PI * 2) + (PI / 2);
	
	return normalize(vec3(sin(rotation), cos(rotation), cos(rotation)*0.4f));
}

vec3 World::getSunColor() {
	float sunIntensity = dot(getSunDirection(), vec3(0.0f, -1.0f, 0.0f)) * 5.0f;

	if (sunIntensity < 0.0f) {
		sunIntensity = 0.0f;
	}
	if (sunIntensity > 1.0f) {
		sunIntensity = 1.0f;
	}


	return vec3(1.4f, 1.3f, 1.0f) * sunIntensity;
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