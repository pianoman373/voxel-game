#include "Chunk.hpp"
#include "World.hpp"
#include "Block.hpp"

#include <crucible/Renderer.hpp>
#include <crucible/Mesh.hpp>
#include <crucible/Math.hpp>

#include <math.h>
#include <queue>

Chunk::Chunk(World &world, int x, int z): world(world) {
    chunk_x = x;
    chunk_z = z;

    this->transform = Transform(vec3(chunk_x * 16, 0.0f, chunk_z * 16), quaternion(), vec3(1.0f));
}

Chunk::~Chunk() {
    mesh.clear();
	mesh.destroy();
}

int rleEncode(uint8_t *input, int inputLength, uint8_t *output) {
    int i = 0;
    int outputIndex = 0;

    bool first = true;


    uint8_t lastVal = input[i] + 1;
    uint8_t count = 1;

    while (true) {
        uint8_t val = input[i];

        if (val == lastVal && count < 255) {
            count++;
        }
        else {
            if (first) {
                first = false;
            }
            else {
                output[outputIndex] = lastVal;
                output[outputIndex+1] = count;
                outputIndex += 2;
            }

            lastVal = val;
            count = 1;
        }

        i++;
        if (i >= inputLength) {
            output[outputIndex] = lastVal;
            output[outputIndex+1] = count;
            outputIndex += 2;

            return outputIndex;
        }
    }
}

int rleDecode(uint8_t *input, int inputLength, uint8_t *output) {
    int outputIndex = 0;

    for (int i = 0; i < (inputLength - 1); i += 2) {
        uint8_t number = input[i];
        uint8_t amount = input[i+1];

        for (int j = 0; j < amount; j++) {
            output[outputIndex] = number;
            outputIndex++;
        }
    }

    return outputIndex;
}

int Chunk::serialize(uint8_t *dataOut) {
    uint32_t *bigData = (uint32_t*)dataOut;

    int blockLength = rleEncode(&blocks[0], 16*16*256, dataOut+8);
    int lightingLength = rleEncode(&lightMap[0], 16*16*256, dataOut+blockLength+8);

    bigData[0] = blockLength;
    bigData[1] = lightingLength;

    return blockLength + lightingLength + 8;
}

void Chunk::unSerialize(uint8_t *dataIn, int dataLength) {
    uint8_t uncompressedBlockData[16*16*256*2] = {0};
    uint8_t uncompressedLightingData[16*16*256*2] = {0};

    uint32_t *bigData = (uint32_t*)dataIn;
    uint32_t blockLength = bigData[0];
    uint32_t lightingLength = bigData[1];

    rleDecode(dataIn+8, blockLength, uncompressedBlockData);
    rleDecode(dataIn+8+blockLength, lightingLength, uncompressedLightingData);

    for (int i = 0; i < 16*16*256; i++) {
        blocks[i] = uncompressedBlockData[i];
        lightMap[i] = uncompressedLightingData[i];
    }
}

char Chunk::getBlock(int x, int y, int z) {
    int index = (y * 16 * 16) + (x * 16) + z;
    if (x > -1 && x < 16 && y > -1 && y < 256 && z > -1 && z < 16) {
		return blocks[index];
    }
	return 1;//cm->getBlock((this->chunk_x * CHUNK_SIZE) + x, (this->chunk_y * CHUNK_SIZE) + y, (this->chunk_z * CHUNK_SIZE) + z);
}

//WIP
int Chunk::getBlockFromWorld(int x, int y, int z) {
    //return cm->getBlock((this->chunk_x * CHUNK_SIZE) + x, (this->chunk_y * CHUNK_SIZE) + y, (this->chunk_z * CHUNK_SIZE) + z);
    return getBlock(x, y, z);
}

void Chunk::setBlock(int x, int y, int z, char block) {
    if (x < 16 && x >= 0 && y < 256 && y >= 0 && z < 16 && z >= 0) {
        isDirty = true;

        int index = (y * 16 * 16) + (x * 16) + z;

        if (blocks[index] != 0 && block == 0) {
            blocks[index] = block;

            std::queue<vec3i> lightBfsQueue;

            for (int i = -1; i < 2; i++) {
                for (int j = -1; j < 2; j++) {
                    for (int k = -1; k < 2; k++) {
                        lightBfsQueue.emplace(x+i, y+j, z+k);
                    }
                }
            }
            //lightBfsQueue.emplace(x, y+1, z);

            ChunkNeighborhood neighborhood = world.getChunkNeighborhood(chunk_x, chunk_z);
            propagateSunlight(neighborhood, lightBfsQueue);

        }
        else if (blocks[index] == 0 && block != 0) {
            blocks[index] = block;

            std::queue<vec4i> lightBfsQueue;
            lightBfsQueue.emplace(x, y, z, 15);

            setSunlight(x, y, z, 0);
            ChunkNeighborhood neighborhood = world.getChunkNeighborhood(chunk_x, chunk_z);
            unPropagateSunlight(neighborhood, lightBfsQueue);
        }
        else {
            blocks[index] = block;
        }
    }
}

// Get the bits XXXX0000

int Chunk::getSunlight(int x, int y, int z) {
    int index = (y * 16 * 16) + (x * 16) + z;

    if (x < 16 && x >= 0 && y < 256 && y >= 0 && z < 16 && z >= 0) {
        return (lightMap[index] >> 4) & 0xF;
    }
    else {
        return 15;
    }
}


// Set the bits XXXX0000
void Chunk::setSunlight(int x, int y, int z, int val) {
    int index = (y * 16 * 16) + (x * 16) + z;

    if (x < 16 && x >= 0 && y < 256 && y >= 0 && z < 16 && z >= 0) {
        isDirty = true;

        lightMap[index] = (lightMap[index] & 0xF) | (val << 4);
    }
    else {

    }
}


// Get the bits 0000XXXX

int Chunk::getTorchlight(int x, int y, int z) {
    int index = (y * 16 * 16) + (x * 16) + z;

    if (x < 16 && x >= 0 && y < 256 && y >= 0 && z < 16 && z >= 0) {
        return lightMap[index] & 0xF;
    }
    else {
        return 15;
    }
}

// Set the bits 0000XXXX
void Chunk::setTorchlight(int x, int y, int z, int val) {
    int index = (y * 16 * 16) + (x * 16) + z;

    if (x < 16 && x >= 0 && y < 256 && y >= 0 && z < 16 && z >= 0) {
        lightMap[index] = (lightMap[index] & 0xF0) | val;
    }
    else {

    }
}

void Chunk::calculateSunLighting() {
    ChunkNeighborhood neighborhood = world.getChunkNeighborhood(chunk_x, chunk_z);

    std::queue<vec3i> lightBfsQueue;


    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            lightBfsQueue.emplace(x, 256, z);
        }
    }

    propagateSunlight(neighborhood, lightBfsQueue);
}

void Chunk::propagateSunlight(ChunkNeighborhood &neighborhood, std::queue<vec3i> &lightBfsQueue) {
    while (!lightBfsQueue.empty()) {
        // Get a reference to the front node.
        vec3i node = lightBfsQueue.front();
        lightBfsQueue.pop();

        int lightLevel = neighborhood.getSunlight(node.x, node.y, node.z);

        if (neighborhood.getBlock(node.x - 1, node.y, node.z) == 0 && neighborhood.getSunlight(node.x - 1, node.y, node.z) + 2 <= lightLevel) {
            neighborhood.setSunlight(node.x - 1, node.y, node.z, lightLevel - 1);

            lightBfsQueue.emplace(node.x - 1, node.y, node.z);
        }
        if (neighborhood.getBlock(node.x + 1, node.y, node.z) == 0 && neighborhood.getSunlight(node.x + 1, node.y, node.z) + 2 <= lightLevel) {
            neighborhood.setSunlight(node.x + 1, node.y, node.z, lightLevel - 1);

            lightBfsQueue.emplace(node.x + 1, node.y, node.z);
        }
        if (neighborhood.getBlock(node.x, node.y - 1, node.z) == 0 && neighborhood.getSunlight(node.x, node.y - 1, node.z) < lightLevel) {
            neighborhood.setSunlight(node.x, node.y - 1, node.z, lightLevel);

            lightBfsQueue.emplace(node.x, node.y - 1, node.z);
        }
        if (neighborhood.getBlock(node.x, node.y + 1, node.z) == 0 && neighborhood.getSunlight(node.x, node.y + 1, node.z) + 2 <= lightLevel) {
            neighborhood.setSunlight(node.x, node.y + 1, node.z, lightLevel - 1);

            lightBfsQueue.emplace(node.x, node.y + 1, node.z);
        }

        if (neighborhood.getBlock(node.x, node.y, node.z - 1) == 0 && neighborhood.getSunlight(node.x, node.y, node.z - 1) + 2 <= lightLevel) {
            neighborhood.setSunlight(node.x, node.y, node.z - 1, lightLevel - 1);

            lightBfsQueue.emplace(node.x, node.y, node.z - 1);
        }
        if (neighborhood.getBlock(node.x, node.y, node.z + 1) == 0 && neighborhood.getSunlight(node.x, node.y, node.z + 1) + 2 <= lightLevel) {
            neighborhood.setSunlight(node.x, node.y, node.z + 1, lightLevel - 1);

            lightBfsQueue.emplace(node.x, node.y, node.z + 1);
        }
    }
}

void Chunk::unPropagateSunlight(ChunkNeighborhood &neighborhood, std::queue<vec4i> &lightBfsQueue) {

    std::queue<vec3i> lightAdditionQueue;

    while (!lightBfsQueue.empty()) {
        // Get a reference to the front node.
        vec4i node = lightBfsQueue.front();
        lightBfsQueue.pop();

        int lightLevel = node.w;

        int neighborLevel;


        neighborLevel = neighborhood.getSunlight(node.x-1, node.y, node.z);
        if (neighborLevel != 0 && neighborLevel < lightLevel) {
            // Set its light level
            neighborhood.setSunlight(node.x-1, node.y, node.z, 0);

            // Emplace new node to queue. (could use push as well)
            lightBfsQueue.emplace(node.x-1, node.y, node.z, lightLevel-1);

        } else if (neighborLevel >= lightLevel) {
            lightAdditionQueue.emplace(node.x-1, node.y, node.z);
        }

        neighborLevel = neighborhood.getSunlight(node.x+1, node.y, node.z);
        if (neighborLevel != 0 && neighborLevel < lightLevel) {
            // Set its light level
            neighborhood.setSunlight(node.x+1, node.y, node.z, 0);

            // Emplace new node to queue. (could use push as well)
            lightBfsQueue.emplace(node.x+1, node.y, node.z, lightLevel-1);

        } else if (neighborLevel >= lightLevel) {
            lightAdditionQueue.emplace(node.x+1, node.y, node.z);
        }



        neighborLevel = neighborhood.getSunlight(node.x, node.y-1, node.z);
        if (neighborLevel != 0) {
            // Set its light level
            neighborhood.setSunlight(node.x, node.y-1, node.z, 0);

            // Emplace new node to queue. (could use push as well)
            lightBfsQueue.emplace(node.x, node.y-1, node.z, lightLevel);

        } else if (neighborLevel >= lightLevel) {
            lightAdditionQueue.emplace(node.x, node.y-1, node.z);
        }

        neighborLevel = neighborhood.getSunlight(node.x, node.y+1, node.z);
        if (neighborLevel != 0 && neighborLevel < lightLevel) {
            // Set its light level
            neighborhood.setSunlight(node.x, node.y+1, node.z, 0);

            // Emplace new node to queue. (could use push as well)
            lightBfsQueue.emplace(node.x, node.y+1, node.z, lightLevel-1);

        } else if (neighborLevel >= lightLevel) {
            lightAdditionQueue.emplace(node.x, node.y+1, node.z);
        }




        neighborLevel = neighborhood.getSunlight(node.x, node.y, node.z-1);
        if (neighborLevel != 0 && neighborLevel < lightLevel) {
            // Set its light level
            neighborhood.setSunlight(node.x, node.y, node.z-1, 0);

            // Emplace new node to queue. (could use push as well)
            lightBfsQueue.emplace(node.x, node.y, node.z-1, lightLevel-1);

        } else if (neighborLevel >= lightLevel) {
            lightAdditionQueue.emplace(node.x, node.y, node.z-1);
        }

        neighborLevel = neighborhood.getSunlight(node.x, node.y, node.z+1);
        if (neighborLevel != 0 && neighborLevel < lightLevel) {
            // Set its light level
            neighborhood.setSunlight(node.x, node.y, node.z+1, 0);

            // Emplace new node to queue. (could use push as well)
            lightBfsQueue.emplace(node.x, node.y, node.z+1, lightLevel-1);

        } else if (neighborLevel >= lightLevel) {
            lightAdditionQueue.emplace(node.x, node.y, node.z+1);
        }
    }

    propagateSunlight(neighborhood, lightAdditionQueue);
}