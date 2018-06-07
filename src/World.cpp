#include "World.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Settings.hpp"
#include "Util.hpp"
#include "Client.hpp"
#include "Chunk.hpp"
#include "SimplexNoise.hpp"

#include <crucible/Renderer.hpp>
#include <crucible/Input.hpp>
#include <crucible/Camera.hpp>
#include <crucible/AABB.hpp>
#include <crucible/Frustum.hpp>

#include <iostream>
#include <mutex>
#include <thread>
#include <algorithm>

static std::vector<vec3i> chunkLoadingPositions;

static float getHeight(int x, int z) {
	return Util::ridgedNoise(vec2(x / 10.0f, z / 10.0f), 5, 0.01f, 0.5f) * 50.0f - 50.0f;
}

char topBlock = 1;    //grass
char midBlock = 3;    //dirt
char fillerBlock = 2; //stone

static void placeTree(ChunkManager &manager, int x, int y, int z) {
    manager.setBlock(x, y, z, 6);
    manager.setBlock(x, y+1, z, 6);
    manager.setBlock(x, y+2, z, 6);
	manager.setBlock(x, y+3, z, 6);
	manager.setBlock(x, y+4, z, 6);
	manager.setBlock(x, y+5, z, 6);
	manager.setBlock(x, y+6, z, 6);

	manager.setBlock(x-1, y+3, z, 7);
	manager.setBlock(x+1, y+3, z, 7);
	manager.setBlock(x, y+3, z-1, 7);
	manager.setBlock(x, y+3, z+1, 7);


	manager.setBlock(x-1, y+4, z, 7);
	manager.setBlock(x+1, y+4, z, 7);
	manager.setBlock(x, y+4, z-1, 7);
	manager.setBlock(x, y+4, z+1, 7);
	manager.setBlock(x-1, y+4, z-1, 7);
	manager.setBlock(x+1, y+4, z+1, 7);
	manager.setBlock(x-1, y+4, z+1, 7);
	manager.setBlock(x+1, y+4, z-1, 7);


	manager.setBlock(x-1, y+5, z, 7);
	manager.setBlock(x+1, y+5, z, 7);
	manager.setBlock(x, y+5, z-1, 7);
	manager.setBlock(x, y+5, z+1, 7);
	manager.setBlock(x-1, y+5, z-1, 7);
	manager.setBlock(x+1, y+5, z+1, 7);
	manager.setBlock(x-1, y+5, z+1, 7);
	manager.setBlock(x+1, y+5, z-1, 7);


	manager.setBlock(x-1, y+6, z, 7);
	manager.setBlock(x+1, y+6, z, 7);
	manager.setBlock(x, y+6, z-1, 7);
	manager.setBlock(x, y+6, z+1, 7);

	manager.setBlock(x, y+7, z, 7);
	manager.setBlock(x-1, y+7, z, 7);
	manager.setBlock(x+1, y+7, z, 7);
	manager.setBlock(x, y+7, z-1, 7);
	manager.setBlock(x, y+7, z+1, 7);

	manager.setBlock(x, y+8, z, 7);
	manager.setBlock(x, y+9, z, 7);
	manager.setBlock(x, y+10, z, 7);
}

static void generateChunk(ChunkManager &manager, Chunk *chunk) {
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
			bool steep = dot(finalNorm, vec3(0, 1, 0)) < 0.8f;

			int actualX = x - 1 + (chunk->chunk_x * 32);
			int actualZ = z - 1 + (chunk->chunk_z * 32);


			float r = SimplexNoise::noise(actualX * 100.0f, actualZ * 100.0f); //tree spawning

			for (int y = 0; y < 32; y++) {

                int actualY = y - 1 + (chunk->chunk_y * 32);



                if (SimplexNoise::noise((float)actualX / 70.0f, (float)actualY / 70.0f, (float)actualZ / 50.0f) > -0.5f) {
					int actualHeight = y + (chunk->chunk_y * 32);

					if (actualHeight < height) {
						if (steep) {
							chunk->setBlock(x, y, z, fillerBlock);
						}
						else if (actualHeight < height - 5) {

							chunk->setBlock(x, y, z, fillerBlock);

						} else if (actualHeight < height - 1) {
							chunk->setBlock(x, y, z, midBlock);
						} else {
                            float n = Util::noise(vec2(actualX / 50.0f, actualZ / 50.0f), 5, 0.1f, 0.5f);

                            if (n > 0.0) {


                                if (r > 0.965f) {
                                    placeTree(manager, x + (chunk->chunk_x * CHUNK_SIZE),
                                              y + (chunk->chunk_y * CHUNK_SIZE) + 1, z + (chunk->chunk_z * CHUNK_SIZE));
                                }
                            }



                            chunk->setBlock(x, y, z, topBlock);


						}
					}
				}
			}
		}
	}
}

void World::chunkUpdateThread() {
	while (running) {
		for (int i = 0; i < chunkLoadingPositions.size(); i++) {
			vec3i cameraChunkPos = worldToChunkPos(Client::camera.position);

			vec3i loadingPos = chunkLoadingPositions[i];

			vec3i pos = loadingPos + cameraChunkPos;

			vec3 chunkPosWorld = vec3(pos.x * CHUNK_SIZE, (pos.y * CHUNK_SIZE), pos.z * CHUNK_SIZE);

			if (Client::frustum.isBoxInside(AABB(chunkPosWorld, chunkPosWorld + vec3(CHUNK_SIZE))) || AABB(chunkPosWorld, chunkPosWorld + vec3(CHUNK_SIZE)).isVecInside(Client::camera.getPosition())) {

			}
			else {
				continue;
			}


			Chunk * chunk = manager.getChunk(pos.x, pos.y, pos.z);

			if (!chunk->generated || chunk->isDirty) {

				for (int xm = -1; xm < 2; xm++) {
					for (int ym = -1; ym < 2; ym++) {
						for (int zm = -1; zm < 2; zm++) {
							Chunk *c = manager.getChunk(pos.x + xm, pos.y + ym, pos.z + zm);
							if (!c->generated) {
								c->generated = true;
								c->isDirty = true;
								generateChunk(manager, c);
							}
						}
					}
				}


                chunk->generateMesh();
				chunk->isDirty = false;

				break;
			}
		}
	}
}

void World::shutdown() {
	running = false;
}

void World::init(const Camera& camera) {
	for (int i = 0; i < Settings::render_distance; i++) {

		for (int x = -i; x <= i; x++) {
			for (int y = -i; y <= i; y++) {
				for (int z = -i; z <= i; z++) {
					if (std::find(chunkLoadingPositions.begin(), chunkLoadingPositions.end(), vec3i(x,y, z)) == chunkLoadingPositions.end()) {
						chunkLoadingPositions.push_back(vec3i(x, y, z));
					}
				}
			}
		}
	}

	std::thread thread0([&]() {
        chunkUpdateThread();
    });
	thread0.detach();
}

void World::render(Camera &cam, Material *nearmaterial) {
	vec3i camPos = World::worldToChunkPos(cam.position);
	bool rebuild = true;

	auto chunksCopy = manager.getChunks();

    for (auto const &ref: chunksCopy) {
        Chunk *c = ref.second;
        vec3i chunkPos = ref.first;

        if (abs(chunkPos.x - camPos.x) > Settings::render_distance+1 || abs(chunkPos.y - camPos.y) > Settings::render_distance+1 || abs(chunkPos.z - camPos.z) > Settings::render_distance+1) {
        	//manager.deleteChunk(chunkPos.x, chunkPos.y, chunkPos.z);
        	continue;
        }

		vec3 chunkPosWorld = vec3(c->chunk_x * CHUNK_SIZE, (c->chunk_y * CHUNK_SIZE), c->chunk_z * CHUNK_SIZE);

		if (!c->empty) {
			c->render(nearmaterial);
		}
    }
}

void World::update(const Camera &cam, float delta) {
	time += delta;

	if (time > DAYLIGHT_CYCLE) {
		time -= DAYLIGHT_CYCLE;
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
	if (sunIntensity > 2.0f) {
		sunIntensity = 2.0f;
	}

	return vec3(1.4f, 1.3f, 1.0f) * sunIntensity;
}

vec3 World::getAmbient() {
	float sunIntensity = dot(getSunDirection(), vec3(0.0f, -1.0f, 0.0f));

	if (sunIntensity < 0.0f) {
		sunIntensity = 0.0f;
	}
    if (sunIntensity > 2.0f) {
        sunIntensity = 2.0f;
    }

	return (vec3(0.5, 0.6, 1.0) * 0.5f * sunIntensity) + (vec3(0.5, 0.6, 1.0) * 0.05f);
}

vec3i World::worldToChunkPos(vec3 pos) {
    return vec3i((int)pos.x >> 5, (int)pos.y >> 5, (int)pos.z >> 5);
}