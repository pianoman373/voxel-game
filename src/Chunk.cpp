#include "Chunk.hpp"
#include "World.hpp"
#include "Block.hpp"
#include "ChunkManager.hpp"

#include <crucible/MeshFactory.hpp>
#include <crucible/Renderer.hpp>
#include <crucible/Mesh.hpp>
#include <crucible/Math.hpp>

#include <math.h>
#include <queue>

Chunk::Chunk(int x, int y, int z, ChunkManager* cm) {
    chunk_x = x;
    chunk_y = y;
    chunk_z = z;

    this->cm = cm;
}

Chunk::~Chunk() {
    mesh.clear();
	mesh.destroy();
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

	if (x < CHUNK_SIZE && x >= 0 && y < CHUNK_SIZE && y >= 0 && z < CHUNK_SIZE && z >= 0) {
		return lightMap[y][z][x] & 0xF;
	}
    else {
//        vec3i worldPosition = vec3i((chunk_x * CHUNK_SIZE) + x, (chunk_y * CHUNK_SIZE) + y, (chunk_z * CHUNK_SIZE) + z);
//        Chunk *c = cm->getChunk(worldPosition.x >> 5, worldPosition.y >> 5, worldPosition.z >> 5);
//        return c->getTorchlight(worldPosition.x & 31, worldPosition.y & 31, worldPosition.z & 31);
        return 15;
    }
}

// Set the bits 0000XXXX
void Chunk::setTorchlight(int x, int y, int z, int val) {
	if (x < CHUNK_SIZE && x >= 0 && y < CHUNK_SIZE && y >= 0 && z < CHUNK_SIZE && z >= 0) {
		lightMap[y][z][x] = (lightMap[x][y][z] & 0xF0) | val;
	}
    else {
        vec3i worldPosition = vec3i((chunk_x * CHUNK_SIZE) + x, (chunk_y * CHUNK_SIZE) + y, (chunk_z * CHUNK_SIZE) + z);
        Chunk *c = cm->getChunk(worldPosition.x >> 5, worldPosition.y >> 5, worldPosition.z >> 5);
        c->setTorchlight(worldPosition.x & 31, worldPosition.y & 31, worldPosition.z & 31, val);
        c->isDirty = true;
    }
}

void Chunk::placeTorch(int x, int y, int z) {
	setTorchlight(x, y, z, 15);

	std::queue <vec3i> lightBfsQueue;
	lightBfsQueue.emplace(x, y, z);

	while (!lightBfsQueue.empty()) {
		// Get a reference to the front node. 
		vec3i node = lightBfsQueue.front(); 
		lightBfsQueue.pop();

		int lightLevel = getTorchlight(node.x, node.y, node.z);
		std::cout << (int)getBlock(node.x - 1, node.y, node.z) << std::endl;

		if (getBlock(node.x - 1, node.y, node.z) == 0 && getTorchlight(node.x - 1, node.y, node.z) + 2 <= lightLevel) {
			setTorchlight(node.x - 1, node.y, node.z, lightLevel - 1);
			
			lightBfsQueue.emplace(node.x - 1, node.y, node.z);
		}
		if (getBlock(node.x + 1, node.y, node.z) == 0 && getTorchlight(node.x + 1, node.y, node.z) + 2 <= lightLevel) {
			setTorchlight(node.x + 1, node.y, node.z, lightLevel - 1);

			lightBfsQueue.emplace(node.x + 1, node.y, node.z);
		}

		if (getBlock(node.x, node.y - 1, node.z) == 0 && getTorchlight(node.x, node.y - 1, node.z) + 2 <= lightLevel) {
			setTorchlight(node.x, node.y - 1, node.z, lightLevel - 1);

			lightBfsQueue.emplace(node.x, node.y - 1, node.z);
		}
		if (getBlock(node.x, node.y + 1, node.z) == 0 && getTorchlight(node.x, node.y + 1, node.z) + 2 <= lightLevel) {
			setTorchlight(node.x, node.y + 1, node.z, lightLevel - 1);

			lightBfsQueue.emplace(node.x, node.y + 1, node.z);
		}

		if (getBlock(node.x, node.y, node.z - 1) == 0 && getTorchlight(node.x, node.y, node.z - 1) + 2 <= lightLevel) {
			setTorchlight(node.x, node.y, node.z - 1, lightLevel - 1);

			lightBfsQueue.emplace(node.x, node.y, node.z - 1);
		}
		if (getBlock(node.x, node.y, node.z + 1) == 0 && getTorchlight(node.x, node.y, node.z + 1) + 2 <= lightLevel) {
			setTorchlight(node.x, node.y, node.z + 1, lightLevel - 1);

			lightBfsQueue.emplace(node.x, node.y, node.z + 1);
		}
	}
}

void Chunk::removeTorch(int x, int y, int z) {
    std::queue <vec4i> lightBfsRemovalQueue;
    lightBfsRemovalQueue.emplace(x, y, z, getTorchlight(x, y, z));

    std::queue <vec3i> lightBfsQueue;



    while (!lightBfsRemovalQueue.empty()) {
        // Get a reference to the front node.
        vec4i node = lightBfsRemovalQueue.front();
        lightBfsRemovalQueue.pop();

        int lightLevel = getTorchlight(node.x, node.y, node.z);

        {
            int neighborLevel = getTorchlight(node.x - 1, node.y, node.z);
            if (neighborLevel != 0 && neighborLevel < lightLevel) {
                //setTorchlight(node.x - 1, node.y, node.z, 0);

                lightBfsRemovalQueue.emplace(node.x - 1, node.y, node.z, neighborLevel);
            } else if (neighborLevel >= lightLevel) {
                lightBfsQueue.emplace(node.x - 1, node.y, node.z);
            }
        }
        {
            int neighborLevel = getTorchlight(node.x + 1, node.y, node.z);
            if (neighborLevel != 0 && neighborLevel < lightLevel) {
                //setTorchlight(node.x + 1, node.y, node.z, 0);

                lightBfsRemovalQueue.emplace(node.x + 1, node.y, node.z, neighborLevel);
            } else if (neighborLevel >= lightLevel) {
                lightBfsQueue.emplace(node.x + 1, node.y, node.z);
            }
        }

        {
            int neighborLevel = getTorchlight(node.x, node.y - 1, node.z);
            if (neighborLevel != 0 && neighborLevel < lightLevel) {
                //setTorchlight(node.x, node.y - 1, node.z, 0);

                lightBfsRemovalQueue.emplace(node.x, node.y - 1, node.z, neighborLevel);
            } else if (neighborLevel >= lightLevel) {
                lightBfsQueue.emplace(node.x, node.y - 1, node.z);
            }
        }
        {
            int neighborLevel = getTorchlight(node.x, node.y + 1, node.z);
            if (neighborLevel != 0 && neighborLevel < lightLevel) {
                //setTorchlight(node.x, node.y + 1, node.z, 0);

                lightBfsRemovalQueue.emplace(node.x, node.y + 1, node.z, neighborLevel);
            } else if (neighborLevel >= lightLevel) {
                lightBfsQueue.emplace(node.x, node.y + 1, node.z);
            }
        }

        {
            int neighborLevel = getTorchlight(node.x, node.y, node.z - 1);
            if (neighborLevel != 0 && neighborLevel < lightLevel) {
                //setTorchlight(node.x, node.y, node.z - 1, 0);

                lightBfsRemovalQueue.emplace(node.x, node.y, node.z - 1, neighborLevel);
            } else if (neighborLevel >= lightLevel) {
                lightBfsQueue.emplace(node.x, node.y, node.z - 1);
            }
        }
        {
            int neighborLevel = getTorchlight(node.x, node.y, node.z + 1);
            if (neighborLevel != 0 && neighborLevel < lightLevel) {
                //setTorchlight(node.x, node.y, node.z + 1, 0);

                lightBfsRemovalQueue.emplace(node.x, node.y, node.z + 1, neighborLevel);
            } else if (neighborLevel >= lightLevel) {
                lightBfsQueue.emplace(node.x, node.y, node.z + 1);
            }
        }

        setTorchlight(node.x, node.y, node.z, 0);

    }

    while (!lightBfsQueue.empty()) {
        // Get a reference to the front node.
        vec3i node = lightBfsQueue.front();
        lightBfsQueue.pop();

        int lightLevel = getTorchlight(node.x, node.y, node.z);

        if (getBlock(node.x - 1, node.y, node.z) == 0 && getTorchlight(node.x - 1, node.y, node.z) + 2 <= lightLevel) {
            setTorchlight(node.x - 1, node.y, node.z, lightLevel - 1);

            lightBfsQueue.emplace(node.x - 1, node.y, node.z);
        }
        if (getBlock(node.x + 1, node.y, node.z) == 0 && getTorchlight(node.x + 1, node.y, node.z) + 2 <= lightLevel) {
            setTorchlight(node.x + 1, node.y, node.z, lightLevel - 1);

            lightBfsQueue.emplace(node.x + 1, node.y, node.z);
        }

        if (getBlock(node.x, node.y - 1, node.z) == 0 && getTorchlight(node.x, node.y - 1, node.z) + 2 <= lightLevel) {
            setTorchlight(node.x, node.y - 1, node.z, lightLevel - 1);

            lightBfsQueue.emplace(node.x, node.y - 1, node.z);
        }
        if (getBlock(node.x, node.y + 1, node.z) == 0 && getTorchlight(node.x, node.y + 1, node.z) + 2 <= lightLevel) {
            setTorchlight(node.x, node.y + 1, node.z, lightLevel - 1);

            lightBfsQueue.emplace(node.x, node.y + 1, node.z);
        }

        if (getBlock(node.x, node.y, node.z - 1) == 0 && getTorchlight(node.x, node.y, node.z - 1) + 2 <= lightLevel) {
            setTorchlight(node.x, node.y, node.z - 1, lightLevel - 1);

            lightBfsQueue.emplace(node.x, node.y, node.z - 1);
        }
        if (getBlock(node.x, node.y, node.z + 1) == 0 && getTorchlight(node.x, node.y, node.z + 1) + 2 <= lightLevel) {
            setTorchlight(node.x, node.y, node.z + 1, lightLevel - 1);

            lightBfsQueue.emplace(node.x, node.y, node.z + 1);
        }
    }
}

char Chunk::getBlock(int x, int y, int z) {
    if (x < CHUNK_SIZE && x >= 0 && y < CHUNK_SIZE && y >= 0 && z < CHUNK_SIZE && z >= 0) {
		return blocks[x + CHUNK_SIZE * (y + CHUNK_SIZE * z)];
    }
	return cm->getBlock((this->chunk_x * CHUNK_SIZE) + x, (this->chunk_y * CHUNK_SIZE) + y, (this->chunk_z * CHUNK_SIZE) + z);
}

//WIP
int Chunk::getBlockFromWorld(int x, int y, int z) {
    //return cm->getBlock((this->chunk_x * CHUNK_SIZE) + x, (this->chunk_y * CHUNK_SIZE) + y, (this->chunk_z * CHUNK_SIZE) + z);
    return getBlock(x, y, z);
}

void Chunk::setBlock(int x, int y, int z, char block) {
    if (x < CHUNK_SIZE && x >= 0 && y < CHUNK_SIZE && y >= 0 && z < CHUNK_SIZE && z >= 0) {
		blocks[x + CHUNK_SIZE * (y + CHUNK_SIZE * z)] = block;

        if (block != 0) {
            if (this->empty) {
                this->empty = false;
            }
        }
    }
}

const float textureSize = 16.0f;
const float AO = 0.3333f;


void Chunk::generateMesh() {
//    std::queue <vec3i> lightBfsQueue;
//
//    for (int x = 0; x < 32; x++) {
//        for (int z = 0; z < 32; z++) {
//
//
//            if (!cm->getChunk(chunk_x, chunk_y + 1, chunk_z)->isMeshGenerated) {
//                lightBfsQueue.emplace(x, 32, z);
//                setTorchlight(x, 32, z, 15);
//            }
//            else {
//                lightBfsQueue.emplace(x, 33, z);
//
//            }
//
//        }
//    }
//
//
//    while (!lightBfsQueue.empty()) {
//        // Get a reference to the front node.
//        vec3i node = lightBfsQueue.front();
//        lightBfsQueue.pop();
//
//        int lightLevel = getTorchlight(node.x, node.y, node.z);
//
//        if (getBlock(node.x - 1, node.y, node.z) == 0 && getTorchlight(node.x - 1, node.y, node.z) + 2 <= lightLevel) {
//            setTorchlight(node.x - 1, node.y, node.z, lightLevel - 1);
//
//            lightBfsQueue.emplace(node.x - 1, node.y, node.z);
//        }
//        if (getBlock(node.x + 1, node.y, node.z) == 0 && getTorchlight(node.x + 1, node.y, node.z) + 2 <= lightLevel) {
//            setTorchlight(node.x + 1, node.y, node.z, lightLevel - 1);
//
//            lightBfsQueue.emplace(node.x + 1, node.y, node.z);
//        }
//
//        if (getBlock(node.x, node.y - 1, node.z) == 0 && getTorchlight(node.x, node.y - 1, node.z) + 2 <= lightLevel) {
//            setTorchlight(node.x, node.y - 1, node.z, lightLevel);
//
//            lightBfsQueue.emplace(node.x, node.y - 1, node.z);
//        }
//        if (getBlock(node.x, node.y + 1, node.z) == 0 && getTorchlight(node.x, node.y + 1, node.z) + 2 <= lightLevel) {
//            setTorchlight(node.x, node.y + 1, node.z, lightLevel - 1);
//
//            lightBfsQueue.emplace(node.x, node.y + 1, node.z);
//        }
//
//        if (getBlock(node.x, node.y, node.z - 1) == 0 && getTorchlight(node.x, node.y, node.z - 1) + 2 <= lightLevel) {
//            setTorchlight(node.x, node.y, node.z - 1, lightLevel - 1);
//
//            lightBfsQueue.emplace(node.x, node.y, node.z - 1);
//        }
//        if (getBlock(node.x, node.y, node.z + 1) == 0 && getTorchlight(node.x, node.y, node.z + 1) + 2 <= lightLevel) {
//            setTorchlight(node.x, node.y, node.z + 1, lightLevel - 1);
//
//            lightBfsQueue.emplace(node.x, node.y, node.z + 1);
//        }
//    }

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            for (int y = 0; y < CHUNK_SIZE; y++) {
                lightMap[x][y][z] = 15;
            }
        }
    }

    if (!empty) {
        float bias = 0.00001f;

        for (int x = 0; x < CHUNK_SIZE; x++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                for (int y = 0; y < CHUNK_SIZE; y++) {

                    int block_X_Y_Z = getBlockFromWorld(x, y, z);

                    if (block_X_Y_Z == 0)
                       continue;

					float lightLevel = 0.0f;


                    Block *block = BlockRegistry::getBlock(block_X_Y_Z);

					


                    bool block_X_posY_Z = !BlockRegistry::getBlock(getBlockFromWorld(x, y + 1, z))->isSolid();
                    bool block_X_negY_Z = !BlockRegistry::getBlock(getBlockFromWorld(x, y - 1, z))->isSolid();

                    bool block_X_Y_negZ = !BlockRegistry::getBlock(getBlockFromWorld(x, y, z - 1))->isSolid();
                    bool block_X_posY_negZ = !BlockRegistry::getBlock(getBlockFromWorld(x, y + 1, z - 1))->isSolid();
                    bool block_X_negY_negZ = !BlockRegistry::getBlock(getBlockFromWorld(x, y - 1, z - 1))->isSolid();

                    bool block_X_Y_posZ = !BlockRegistry::getBlock(getBlockFromWorld(x, y, z + 1))->isSolid();
                    bool block_X_posY_posZ = !BlockRegistry::getBlock(getBlockFromWorld(x, y + 1, z + 1))->isSolid();
                    bool block_X_negY_posZ = !BlockRegistry::getBlock(getBlockFromWorld(x, y - 1, z + 1))->isSolid();


                    bool block_posX_Y_Z = !BlockRegistry::getBlock(getBlockFromWorld(x + 1, y, z))->isSolid();
                    bool block_posX_posY_Z = !BlockRegistry::getBlock(getBlockFromWorld(x + 1, y + 1, z))->isSolid();
                    bool block_posX_negY_Z = !BlockRegistry::getBlock(getBlockFromWorld(x + 1, y - 1, z))->isSolid();

                    bool block_posX_Y_negZ = !BlockRegistry::getBlock(getBlockFromWorld(x + 1, y, z - 1))->isSolid();
                    bool block_posX_posY_negZ = !BlockRegistry::getBlock(getBlockFromWorld(x + 1, y + 1, z - 1))->isSolid();
                    bool block_posX_negY_negZ = !BlockRegistry::getBlock(getBlockFromWorld(x + 1, y - 1, z - 1))->isSolid();

                    bool block_posX_Y_posZ = !BlockRegistry::getBlock(getBlockFromWorld(x + 1, y, z + 1))->isSolid();
                    bool block_posX_posY_posZ = !BlockRegistry::getBlock(getBlockFromWorld(x + 1, y + 1, z + 1))->isSolid();
                    bool block_posX_negY_posZ = !BlockRegistry::getBlock(getBlockFromWorld(x + 1, y - 1, z + 1))->isSolid();


                    bool block_negX_Y_Z = !BlockRegistry::getBlock(getBlockFromWorld(x - 1, y, z))->isSolid();
                    bool block_negX_posY_Z = !BlockRegistry::getBlock(getBlockFromWorld(x - 1, y + 1, z))->isSolid();
                    bool block_negX_negY_Z = !BlockRegistry::getBlock(getBlockFromWorld(x - 1, y - 1, z))->isSolid();

                    bool block_negX_Y_negZ = !BlockRegistry::getBlock(getBlockFromWorld(x - 1, y, z - 1))->isSolid();
                    bool block_negX_posY_negZ = !BlockRegistry::getBlock(getBlockFromWorld(x - 1, y + 1, z - 1))->isSolid();
                    bool block_negX_negY_negZ = !BlockRegistry::getBlock(getBlockFromWorld(x - 1, y - 1, z - 1))->isSolid();

                    bool block_negX_Y_posZ = !BlockRegistry::getBlock(getBlockFromWorld(x - 1, y, z + 1))->isSolid();
                    bool block_negX_posY_posZ = !BlockRegistry::getBlock(getBlockFromWorld(x - 1, y + 1, z + 1))->isSolid();
                    bool block_negX_negY_posZ = !BlockRegistry::getBlock(getBlockFromWorld(x - 1, y - 1, z + 1))->isSolid();

                    if (block_posX_Y_Z) { //+x face
                        vec2i textureCoord = block->getTextureCoord(EnumDirection::POSITIVE_X);

                        vec2 uv1 = vec2((textureCoord.x / textureSize), (textureCoord.y / textureSize)) + bias;
                        vec2 uv2 = vec2(((textureCoord.x + 1) / textureSize), ((textureCoord.y + 1) / textureSize)) - bias;

                        float ao100 = 1.0f;
                        float ao101 = 1.0f;
                        float ao110 = 1.0f;
                        float ao111 = 1.0f;

                         if (!block_posX_negY_negZ && (block_posX_Y_negZ && block_posX_negY_Z)) ao100 -= AO;
                         if (!block_posX_Y_negZ) ao100 -= AO;
                         if (!block_posX_negY_Z) ao100 -= AO;

                         if (!block_posX_negY_posZ && (block_posX_Y_posZ && block_posX_negY_Z)) ao101 -= AO;
                         if (!block_posX_Y_posZ) ao101 -= AO;
                         if (!block_posX_negY_Z) ao101 -= AO;

                         if (!block_posX_posY_negZ && (block_posX_Y_negZ && block_posX_posY_Z)) ao110 -= AO;
                         if (!block_posX_Y_negZ) ao110 -= AO;
                         if (!block_posX_posY_Z) ao110 -= AO;

                         if (!block_posX_posY_posZ && (block_posX_Y_posZ && block_posX_posY_Z)) ao111 -= AO;
                         if (!block_posX_Y_posZ) ao111 -= AO;
                         if (!block_posX_posY_Z) ao111 -= AO;

						vec3 blockColor = vec3(1.0f) * ((float)getTorchlight(x + 1, y, z) / 15.0f);

                        mesh.pushVertex(1.0f+x, 0.0f+y, 0.0f+z,  uv1.x, uv2.y,  ao100,  0);
                        mesh.pushVertex(1.0f+x, 1.0f+y, 0.0f+z,  uv1.x, uv1.y,  ao110,  0);
                        mesh.pushVertex(1.0f+x, 1.0f+y, 1.0f+z,  uv2.x, uv1.y,  ao111,  0);

                        mesh.pushVertex(1.0f+x, 1.0f+y, 1.0f+z,  uv2.x, uv1.y,  ao111,  0);
                        mesh.pushVertex(1.0f+x, 0.0f+y, 1.0f+z,  uv2.x, uv2.y,  ao101,  0);
                        mesh.pushVertex(1.0f+x, 0.0f+y, 0.0f+z,  uv1.x, uv2.y,  ao100,  0);
                    }
                    if (block_negX_Y_Z) { //-x face
                        vec2i textureCoord = block->getTextureCoord(EnumDirection::NEGATIVE_X);

                        vec2 uv1 = vec2((textureCoord.x / textureSize), (textureCoord.y / textureSize)) + bias;
                        vec2 uv2 = vec2(((textureCoord.x + 1) / textureSize), ((textureCoord.y + 1) / textureSize)) - bias;

                        float ao000 = 1.0f;
                        float ao001 = 1.0f;
                        float ao010 = 1.0f;
                        float ao011 = 1.0f;

                         if (!block_negX_negY_negZ && (block_negX_Y_negZ && block_negX_negY_Z)) ao000 -= AO;
                         if (!block_negX_Y_negZ) ao000 -= AO;
                         if (!block_negX_negY_Z) ao000 -= AO;

                         if (!block_negX_negY_posZ && (block_negX_Y_posZ && block_negX_negY_Z)) ao001 -= AO;
                         if (!block_negX_Y_posZ) ao001 -= AO;
                         if (!block_negX_negY_Z) ao001 -= AO;

                         if (!block_negX_posY_negZ && (block_negX_Y_negZ && block_negX_posY_Z)) ao010 -= AO;
                         if (!block_negX_Y_negZ) ao010 -= AO;
                         if (!block_negX_posY_Z) ao010 -= AO;

                         if (!block_negX_posY_posZ && (block_negX_Y_posZ && block_negX_posY_Z)) ao011 -= AO;
                         if (!block_negX_Y_posZ) ao011 -= AO;
                         if (!block_negX_posY_Z) ao011 -= AO;

						 vec3 blockColor = vec3(1.0f) * ((float)getTorchlight(x - 1, y, z) / 15.0f);

                        mesh.pushVertex( 0.0f+x, 1.0f+y, 1.0f+z,  uv1.x, uv1.y,  ao011,  1);
                        mesh.pushVertex( 0.0f+x, 1.0f+y, 0.0f+z,  uv2.x, uv1.y,  ao010,  1);
                        mesh.pushVertex( 0.0f+x, 0.0f+y, 0.0f+z,  uv2.x, uv2.y,  ao000,  1);

                        mesh.pushVertex( 0.0f+x, 0.0f+y, 0.0f+z,  uv2.x, uv2.y,  ao000,  1);
                        mesh.pushVertex( 0.0f+x, 0.0f+y, 1.0f+z,  uv1.x, uv2.y,  ao001,  1);
                        mesh.pushVertex( 0.0f+x, 1.0f+y, 1.0f+z,  uv1.x, uv1.y,  ao011,  1);
                    }

                    if (block_X_posY_Z) { //top face
                        vec2i textureCoord = block->getTextureCoord(EnumDirection::POSITIVE_Y);

                        vec2 uv1 = vec2((textureCoord.x / textureSize), (textureCoord.y / textureSize)) + bias;
                        vec2 uv2 = vec2(((textureCoord.x + 1) / textureSize), ((textureCoord.y + 1) / textureSize)) - bias;

                        float ao010 = 1.0f;
                        float ao110 = 1.0f;
                        float ao011 = 1.0f;
                        float ao111 = 1.0f;

                         if (!block_negX_posY_negZ && (block_X_posY_negZ && block_negX_posY_Z)) ao010 -= AO;
                         if (!block_X_posY_negZ) ao010 -= AO;
                         if (!block_negX_posY_Z) ao010 -= AO;

                         if (!block_posX_posY_negZ && (block_X_posY_negZ && block_posX_posY_Z)) ao110 -= AO;
                         if (!block_X_posY_negZ) ao110 -= AO;
                         if (!block_posX_posY_Z) ao110 -= AO;

                         if (!block_negX_posY_posZ && (block_X_posY_posZ && block_negX_posY_Z)) ao011 -= AO;
                         if (!block_X_posY_posZ) ao011 -= AO;
                         if (!block_negX_posY_Z) ao011 -= AO;

                         if (!block_posX_posY_posZ && (block_X_posY_posZ && block_posX_posY_Z)) ao111 -= AO;
                         if (!block_X_posY_posZ) ao111 -= AO;
                         if (!block_posX_posY_Z) ao111 -= AO;

						 vec3 blockColor = vec3(1.0f) * ((float)getTorchlight(x, y + 1, z) / 15.0f);

                        mesh.pushVertex( 1.0f+x, 1.0f+y, 1.0f+z,  uv2.x, uv1.y,  ao111,  2);
                        mesh.pushVertex( 1.0f+x, 1.0f+y, 0.0f+z,  uv2.x, uv2.y,  ao110,  2);
                        mesh.pushVertex( 0.0f+x, 1.0f+y, 0.0f+z,  uv1.x, uv2.y,  ao010,  2);

                        mesh.pushVertex( 0.0f+x, 1.0f+y, 0.0f+z,  uv1.x, uv2.y,  ao010,  2);
                        mesh.pushVertex( 0.0f+x, 1.0f+y, 1.0f+z,  uv1.x, uv1.y,  ao011,  2);
                        mesh.pushVertex( 1.0f+x, 1.0f+y, 1.0f+z,  uv2.x, uv1.y,  ao111,  2);
                    }
                    if (block_X_negY_Z) { //bottom face
                        vec2i textureCoord = block->getTextureCoord(EnumDirection::NEGATIVE_Y);

                        vec2 uv1 = vec2((textureCoord.x / textureSize), (textureCoord.y / textureSize)) + bias;
                        vec2 uv2 = vec2(((textureCoord.x + 1) / textureSize), ((textureCoord.y + 1) / textureSize)) - bias;

                        float ao000 = 1.0f;
                        float ao100 = 1.0f;
                        float ao001 = 1.0f;
                        float ao101 = 1.0f;

                         if (!block_negX_negY_negZ && (block_X_negY_negZ && block_negX_negY_Z)) ao000 -= AO;
                         if (!block_X_negY_negZ) ao000 -= AO;
                         if (!block_negX_negY_Z) ao000 -= AO;

                         if (!block_posX_negY_negZ && (block_X_negY_negZ && block_posX_negY_Z)) ao100 -= AO;
                         if (!block_X_negY_negZ) ao100 -= AO;
                         if (!block_posX_negY_Z) ao100 -= AO;

                         if (!block_negX_negY_posZ  && (block_X_negY_posZ && block_negX_negY_Z)) ao001 -= AO;
                         if (!block_X_negY_posZ) ao001 -= AO;
                         if (!block_negX_negY_Z) ao001 -= AO;

                         if (!block_posX_negY_posZ  && (block_X_negY_posZ && block_posX_negY_Z)) ao101 -= AO;
                         if (!block_X_negY_posZ) ao101 -= AO;
                         if (!block_posX_negY_Z) ao101 -= AO;

						 vec3 blockColor = vec3(1.0f) * ((float)getTorchlight(x, y - 1, z) / 15.0f);

                        mesh.pushVertex( 0.0f+x, 0.0f+y, 0.0f+z,  uv1.x, uv2.y,  ao000,  3);
                        mesh.pushVertex( 1.0f+x, 0.0f+y, 0.0f+z,  uv2.x, uv2.y,  ao100,  3);
                        mesh.pushVertex( 1.0f+x, 0.0f+y, 1.0f+z,  uv2.x, uv1.y,  ao101,  3);

                        mesh.pushVertex( 1.0f+x, 0.0f+y, 1.0f+z,  uv2.x, uv1.y,  ao101,  3);
                        mesh.pushVertex( 0.0f+x, 0.0f+y, 1.0f+z,  uv1.x, uv1.y,  ao001,  3);
                        mesh.pushVertex( 0.0f+x, 0.0f+y, 0.0f+z,  uv1.x, uv2.y,  ao000,  3);
                    }
                    if (block_X_Y_posZ) { //+z face
                        vec2i textureCoord = block->getTextureCoord(EnumDirection::POSITIVE_Z);

                        vec2 uv1 = vec2((textureCoord.x / textureSize), (textureCoord.y / textureSize)) + bias;
                        vec2 uv2 = vec2(((textureCoord.x + 1) / textureSize), ((textureCoord.y + 1) / textureSize)) - bias;

                        float ao001 = 1.0f;
                        float ao011 = 1.0f;
                        float ao101 = 1.0f;
                        float ao111 = 1.0f;

                        if (!block_negX_negY_posZ && (block_X_negY_posZ && block_negX_Y_posZ)) ao001 -= AO;
                        if (!block_X_negY_posZ) ao001 -= AO;
                        if (!block_negX_Y_posZ) ao001 -= AO;

                        if (!block_negX_posY_posZ && (block_X_posY_posZ && block_negX_Y_posZ)) ao011 -= AO;
                        if (!block_X_posY_posZ) ao011 -= AO;
                        if (!block_negX_Y_posZ) ao011 -= AO;

                        if (!block_posX_negY_posZ && (block_X_negY_posZ && block_posX_Y_posZ)) ao101 -= AO;
                        if (!block_X_negY_posZ) ao101 -= AO;
                        if (!block_posX_Y_posZ) ao101 -= AO;

                        if (!block_posX_posY_posZ && (block_X_posY_posZ && block_posX_Y_posZ)) ao111 -= AO;
                        if (!block_X_posY_posZ) ao111 -= AO;
                        if (!block_posX_Y_posZ) ao111 -= AO;

                        vec3 blockColor = vec3(1.0f) * ((float)getTorchlight(x, y, z + 1) / 15.0f);

                        mesh.pushVertex( 0.0f+x, 0.0f+y, 1.0f+z,  uv1.x, uv2.y,  ao001,  4);
                        mesh.pushVertex( 1.0f+x, 0.0f+y, 1.0f+z,  uv2.x, uv2.y,  ao101,  4);
                        mesh.pushVertex( 1.0f+x, 1.0f+y, 1.0f+z,  uv2.x, uv1.y,  ao111,  4);

                        mesh.pushVertex( 1.0f+x, 1.0f+y, 1.0f+z,  uv2.x, uv1.y,  ao111,  4);
                        mesh.pushVertex( 0.0f+x, 1.0f+y, 1.0f+z,  uv1.x, uv1.y,  ao011,  4);
                        mesh.pushVertex( 0.0f+x, 0.0f+y, 1.0f+z,  uv1.x, uv2.y,  ao001,  4);
                    }
                    if (block_X_Y_negZ) { //-z face
                        vec2i textureCoord = block->getTextureCoord(EnumDirection::NEGATIVE_Z);

                        vec2 uv1 = vec2((textureCoord.x / textureSize), (textureCoord.y / textureSize)) + bias;
                        vec2 uv2 = vec2(((textureCoord.x + 1) / textureSize), ((textureCoord.y + 1) / textureSize)) - bias;

                        float ao000 = 1.0f;
                        float ao010 = 1.0f;
                        float ao100 = 1.0f;
                        float ao110 = 1.0f;

                         if (!block_negX_negY_negZ && (block_X_negY_negZ && block_negX_Y_negZ)) ao000 -= AO;
                         if (!block_X_negY_negZ) ao000 -= AO;
                         if (!block_negX_Y_negZ) ao000 -= AO;

                         if (!block_negX_posY_negZ && (block_X_posY_negZ && block_negX_Y_negZ)) ao010 -= AO;
                         if (!block_X_posY_negZ) ao010 -= AO;
                         if (!block_negX_Y_negZ) ao010 -= AO;

                         if (!block_posX_negY_negZ && (block_X_negY_negZ && block_posX_Y_negZ)) ao100 -= AO;
                         if (!block_X_negY_negZ) ao100 -= AO;
                         if (!block_posX_Y_negZ) ao100 -= AO;

                         if (!block_posX_posY_negZ && (block_X_posY_negZ && block_posX_Y_negZ)) ao110 -= AO;
                         if (!block_X_posY_negZ) ao110 -= AO;
                         if (!block_posX_Y_negZ) ao110 -= AO;

						 vec3 blockColor = vec3(1.0f) * ((float)getTorchlight(x, y, z - 1) / 15.0f);

                        mesh.pushVertex( 1.0f+x, 1.0f+y, 0.0f+z,  uv2.x, uv1.y,  ao110,  5);
                        mesh.pushVertex( 1.0f+x, 0.0f+y, 0.0f+z,  uv2.x, uv2.y,  ao100,  5);
                        mesh.pushVertex( 0.0f+x, 0.0f+y, 0.0f+z,  uv1.x, uv2.y,  ao000,  5);

                        mesh.pushVertex( 0.0f+x, 0.0f+y, 0.0f+z,  uv1.x, uv2.y,  ao000,  5);
                        mesh.pushVertex( 0.0f+x, 1.0f+y, 0.0f+z,  uv1.x, uv1.y,  ao010,  5);
                        mesh.pushVertex( 1.0f+x, 1.0f+y, 0.0f+z,  uv2.x, uv1.y,  ao110,  5);
                    }
                }
            }
        }
    }

    this->rebuild = true;
}

void Chunk::render(Material *mat) {
	if (this->rebuild) {
		mesh.generate();
		//mesh.clear();

		this->rebuild = false;
	}

	if (this->mesh.data.size() > 0) {
        vec3 chunkPos = vec3(chunk_x * CHUNK_SIZE, (chunk_y * CHUNK_SIZE), chunk_z * CHUNK_SIZE);
        Renderer::render(&this->mesh, mat, Transform(chunkPos, vec3(), vec3(1.0f)), AABB(chunkPos, chunkPos + vec3(CHUNK_SIZE)));
        //Renderer::debug.renderDebugAABB(AABB(chunkPos, chunkPos + vec3(CHUNK_SIZE)), vec3(1.0f, 0.0f, 0.0f));
	}
}
