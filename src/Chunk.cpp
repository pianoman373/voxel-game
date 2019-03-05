#include "Chunk.hpp"
#include "World.hpp"
#include "Block.hpp"

#include <crucible/Renderer.hpp>
#include <crucible/Mesh.hpp>
#include <crucible/Math.hpp>

#include <math.h>
#include <queue>

Chunk::Chunk(int x, int z) {
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
    int length = rleEncode(&blocks[0], 16*16*256, dataOut);

    return length + 1;
}

void Chunk::unSerialize(uint8_t *dataIn, int dataLength) {
    uint8_t uncompressedChunk[16*16*256*16] = {0};

    int length = rleDecode(dataIn, dataLength, uncompressedChunk);

    for (int i = 0; i < 16*16*256; i++) {
        blocks[i] = uncompressedChunk[i];
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
		blocks[(y * 16 * 16) + (x * 16) + z] = block;
    }
}

// Get the bits XXXX0000

int Chunk::getSunlight(int x, int y, int z) {
    return (lightMap[y][z][x] >> 4) & 0xF;
}


// Set the bits XXXX0000
void Chunk::setSunlight(int x, int y, int z, int val) {
    lightMap[y][z][x] = (lightMap[x][y][z] & 0xF) | (val << 4);
}


// Get the bits 0000XXXX

int Chunk::getTorchlight(int x, int y, int z) {
    if (x < 16 && x >= 0 && y < 256 && y >= 0 && z < 16 && z >= 0) {
        return lightMap[y][z][x] & 0xF;
    }
    else {
        return 15;
    }
}

// Set the bits 0000XXXX
void Chunk::setTorchlight(int x, int y, int z, int val) {
    if (x < 16 && x >= 0 && y < 256 && y >= 0 && z < 16 && z >= 0) {
        lightMap[y][z][x] = (lightMap[x][y][z] & 0xF0) | val;
    }
    else {

    }
}