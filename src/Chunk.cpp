#include "Chunk.hpp"
#include "MeshFactory.hpp"
#include "World.hpp"
#include "Renderer.hpp"
#include "Block.hpp"
#include "Math.hpp"

#include <math.h>

Chunk::Chunk(int x, int y, int z, World* world) {
    chunk_x = x;
    chunk_y = y;
    chunk_z = z;

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                blocks[x][y][z] = 0;
            }
        }
    }

    this->world = world;
}

Chunk::~Chunk() {

}

char Chunk::getBlock(int x, int y, int z) {
    if (x < CHUNK_SIZE && x >= 0 && y < CHUNK_SIZE && y >= 0 && z < CHUNK_SIZE && z >= 0) {
        return blocks[x][y][z];
    }
    return 0;
}

//WIP
int Chunk::getBlockFromWorld(int x, int y, int z) {
    //return world->getBlock((this->chunk_x * CHUNK_SIZE) + x, (this->chunk_y * CHUNK_SIZE) + y, (this->chunk_z * CHUNK_SIZE) + z);
    return getBlock(x, y, z);
}

void Chunk::setBlock(int x, int y, int z, char block) {
    if (this->empty) this->empty = false;

    if (x < CHUNK_SIZE && x >= 0 && y < CHUNK_SIZE && y >= 0 && z < CHUNK_SIZE && z >= 0) {
        blocks[x][y][z] = block;
    }
}

//float random helper, should be moved to Util
float frand() {
    return (float)std::rand() / (float)RAND_MAX;
}

void Chunk::generateMesh() {
    if (!empty) {
        MeshFactory ms = MeshFactory();

        float bias = 0.0001f;

        //float textureSize = 64;

        for (int x = 0; x < CHUNK_SIZE; x++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                for (int y = 0; y < CHUNK_SIZE; y++) {
                    vec3 blockColor = vec3(0.4f * (1+(frand()*0.1f-0.05f)), 0.6f * (1+(frand()*0.1f-0.05f)), 0.3f * (1+(frand()*0.1f-0.05f)));

                    int block_X_Y_Z = getBlockFromWorld(x, y, z);

                    Block *block = BlockRegistry::getBlock(block_X_Y_Z);


//                    int block_X_posY_Z = getBlockFromWorld(x, y + 1, z);
//                    int block_X_negY_Z = getBlockFromWorld(x, y - 1, z);

//                    int block_X_Y_negZ = getBlockFromWorld(x, y, z - 1);
//                    int block_X_posY_negZ = getBlockFromWorld(x, y + 1, z - 1);
//                    int block_X_negY_negZ = getBlockFromWorld(x, y - 1, z - 1);

//                    int block_X_Y_posZ = getBlockFromWorld(x, y, z + 1);
//                    int block_X_posY_posZ = getBlockFromWorld(x, y + 1, z + 1);
//                    int block_X_negY_posZ = getBlockFromWorld(x, y - 1, z + 1);


//                    int block_posX_Y_Z = getBlockFromWorld(x + 1, y, z);
//                    int block_posX_posY_Z = getBlockFromWorld(x + 1, y + 1, z);
//                    int block_posX_negY_Z = getBlockFromWorld(x + 1, y - 1, z);

//                    int block_posX_Y_negZ = getBlockFromWorld(x + 1, y, z - 1);
//                    int block_posX_posY_negZ = getBlockFromWorld(x + 1, y + 1, z - 1);
//                    int block_posX_negY_negZ = getBlockFromWorld(x + 1, y - 1, z - 1);

//                    int block_posX_Y_posZ = getBlockFromWorld(x + 1, y, z + 1);
//                    int block_posX_posY_posZ = getBlockFromWorld(x + 1, y + 1, z + 1);
//                    int block_posX_negY_posZ = getBlockFromWorld(x + 1, y - 1, z + 1);


//                    int block_negX_Y_Z = getBlockFromWorld(x - 1, y, z);
//                    int block_negX_posY_Z = getBlockFromWorld(x - 1, y + 1, z);
//                    int block_negX_negY_Z = getBlockFromWorld(x - 1, y - 1, z);

//                    int block_negX_Y_negZ = getBlockFromWorld(x - 1, y, z - 1);
//                    int block_negX_posY_negZ = getBlockFromWorld(x - 1, y + 1, z - 1);
//                    int block_negX_negY_negZ = getBlockFromWorld(x - 1, y - 1, z - 1);

//                    int block_negX_Y_posZ = getBlockFromWorld(x - 1, y, z + 1);
//                    int block_negX_posY_posZ = getBlockFromWorld(x - 1, y + 1, z + 1);
//                    int block_negX_negY_posZ = getBlockFromWorld(x - 1, y - 1, z + 1);

                    if (getBlockFromWorld(x + 1, y, z) == 0 && block_X_Y_Z != 0) { //+x face
                        vec2i textureCoord = block->getTextureCoord(EnumDirection::POSITIVE_X);

                        vec2 uv1 = vec2((textureCoord.x / 8.0f), (textureCoord.y / 8.0f)) + bias;
                        vec2 uv2 = vec2(((textureCoord.x + 1) / 8.0f), ((textureCoord.y + 1) / 8.0f)) - bias;

                        float ao1 = 1.0f; //neg y pos z
                        float ao2 = 1.0f; //pos y pos z
                        float ao3 = 1.0f; //neg y neg z
                        float ao4 = 1.0f; //pos y neg z

                        if (getBlockFromWorld(x + 1, y - 1, z) != 0 || getBlockFromWorld(x + 1, y - 1, z + 1) != 0 || getBlockFromWorld(x + 1, y, z + 1) != 0)
                            ao1 = 0.8f;
                        if (getBlockFromWorld(x + 1, y + 1, z) != 0 || getBlockFromWorld(x + 1, y + 1, z + 1) != 0 || getBlockFromWorld(x + 1, y, z + 1) != 0)
                            ao2 = 0.8f;
                        if (getBlockFromWorld(x + 1, y - 1, z) != 0 || getBlockFromWorld(x + 1, y - 1, z - 1) != 0 || getBlockFromWorld(x + 1, y, z - 1) != 0)
                            ao3 = 0.8f;
                        if (getBlockFromWorld(x + 1, y + 1, z) != 0 || getBlockFromWorld(x + 1, y + 1, z - 1) != 0 || getBlockFromWorld(x + 1, y, z - 1) != 0)
                            ao4 = 0.8f;

                        ms.vertex(1.0f+x,  0.0f+y,  0.0f+z,  1.0f, 0.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao3,  uv1.x, uv2.y);
                        ms.vertex(1.0f+x,  1.0f+y,  0.0f+z,  1.0f, 0.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao4,  uv1.x, uv1.y);
                        ms.vertex(1.0f+x,  1.0f+y,  1.0f+z,  1.0f, 0.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao2,  uv2.x, uv1.y);

                        ms.vertex(1.0f+x,  1.0f+y,  1.0f+z,  1.0f, 0.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao2,  uv2.x, uv1.y);
                        ms.vertex(1.0f+x,  0.0f+y,  1.0f+z,  1.0f, 0.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao1,  uv2.x, uv2.y);
                        ms.vertex(1.0f+x,  0.0f+y,  0.0f+z,  1.0f, 0.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao3,  uv1.x, uv2.y);
                    }
                    if (getBlockFromWorld(x - 1, y, z) == 0 && block_X_Y_Z != 0) { //-x face
                        vec2i textureCoord = block->getTextureCoord(EnumDirection::NEGATIVE_X);

                        vec2 uv1 = vec2((textureCoord.x / 8.0f), (textureCoord.y / 8.0f)) + bias;
                        vec2 uv2 = vec2(((textureCoord.x + 1) / 8.0f), ((textureCoord.y + 1) / 8.0f)) - bias;

                        float ao1 = 1.0f; //neg y pos z
                        float ao2 = 1.0f; //pos y pos z
                        float ao3 = 1.0f; //neg y neg z
                        float ao4 = 1.0f; //pos y neg z

                        if (getBlockFromWorld(x - 1, y - 1, z) != 0 || getBlockFromWorld(x - 1, y - 1, z + 1) != 0 || getBlockFromWorld(x - 1, y, z + 1) != 0)
                            ao1 = 0.8f;
                        if (getBlockFromWorld(x - 1, y + 1, z) != 0 || getBlockFromWorld(x - 1, y + 1, z + 1) != 0 || getBlockFromWorld(x - 1, y, z + 1) != 0)
                            ao2 = 0.8f;
                        if (getBlockFromWorld(x - 1, y - 1, z) != 0 || getBlockFromWorld(x - 1, y - 1, z - 1) != 0 || getBlockFromWorld(x - 1, y, z - 1) != 0)
                            ao3 = 0.8f;
                        if (getBlockFromWorld(x - 1, y + 1, z) != 0 || getBlockFromWorld(x - 1, y + 1, z - 1) != 0 || getBlockFromWorld(x - 1, y, z - 1) != 0)
                            ao4 = 0.8f;

                        ms.vertex( 0.0f+x,  1.0f+y,  1.0f+z,  -1.0f, 0.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao2,  uv1.x, uv1.y);
                        ms.vertex( 0.0f+x,  1.0f+y,  0.0f+z,  -1.0f, 0.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao4,  uv2.x, uv1.y);
                        ms.vertex( 0.0f+x,  0.0f+y,  0.0f+z,  -1.0f, 0.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao3,  uv2.x, uv2.y);

                        ms.vertex( 0.0f+x,  0.0f+y,  0.0f+z,  -1.0f, 0.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao3,  uv2.x, uv2.y);
                        ms.vertex( 0.0f+x,  0.0f+y,  1.0f+z,  -1.0f, 0.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao1,  uv1.x, uv2.y);
                        ms.vertex( 0.0f+x,  1.0f+y,  1.0f+z,  -1.0f, 0.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao2,  uv1.x, uv1.y);
                    }

                    if (getBlockFromWorld(x, y + 1, z) == 0 && block_X_Y_Z != 0) { //top face
                        vec2i textureCoord = block->getTextureCoord(EnumDirection::POSITIVE_Y);

                        vec2 uv1 = vec2((textureCoord.x / 8.0f), (textureCoord.y / 8.0f)) + bias;
                        vec2 uv2 = vec2(((textureCoord.x + 1) / 8.0f), ((textureCoord.y + 1) / 8.0f)) - bias;

                        float ao1 = 1.0f; //neg x pos z
                        float ao2 = 1.0f; //pos x pos z
                        float ao3 = 1.0f; //neg x neg z
                        float ao4 = 1.0f; //pos x neg z

                        if (getBlockFromWorld(x - 1, y + 1, z) != 0 || getBlockFromWorld(x - 1, y + 1, z + 1) != 0 || getBlockFromWorld(x, y + 1, z + 1) != 0)
                            ao1 = 0.8f;
                        if (getBlockFromWorld(x + 1, y + 1, z) != 0 || getBlockFromWorld(x + 1, y + 1, z + 1) != 0 || getBlockFromWorld(x, y + 1, z + 1) != 0)
                            ao2 = 0.8f;
                        if (getBlockFromWorld(x - 1, y + 1, z) != 0 || getBlockFromWorld(x - 1, y + 1, z - 1) != 0 || getBlockFromWorld(x, y + 1, z - 1) != 0)
                            ao3 = 0.8f;
                        if (getBlockFromWorld(x + 1, y + 1, z) != 0 || getBlockFromWorld(x + 1, y + 1, z - 1) != 0 || getBlockFromWorld(x, y + 1, z - 1) != 0)
                            ao4 = 0.8f;

                        ms.vertex( 1.0f+x,  1.0f+y,  1.0f+z,  0.0f, 1.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao2,  uv2.x, uv1.y);
                        ms.vertex( 1.0f+x,  1.0f+y,  0.0f+z,  0.0f, 1.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao4,  uv2.x, uv2.y);
                        ms.vertex( 0.0f+x,  1.0f+y,  0.0f+z,  0.0f, 1.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao3,  uv1.x, uv2.y);

                        ms.vertex( 0.0f+x,  1.0f+y,  0.0f+z,  0.0f, 1.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao3,  uv1.x, uv2.y);
                        ms.vertex( 0.0f+x,  1.0f+y,  1.0f+z,  0.0f, 1.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao1,  uv1.x, uv1.y);
                        ms.vertex( 1.0f+x,  1.0f+y,  1.0f+z,  0.0f, 1.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao2,  uv2.x, uv1.y);
                    }
                    if (getBlockFromWorld(x, y - 1, z) == 0 && block_X_Y_Z != 0) { //bottom face
                        vec2i textureCoord = block->getTextureCoord(EnumDirection::NEGATIVE_Y);

                        vec2 uv1 = vec2((textureCoord.x / 8.0f), (textureCoord.y / 8.0f)) + bias;
                        vec2 uv2 = vec2(((textureCoord.x + 1) / 8.0f), ((textureCoord.y + 1) / 8.0f)) - bias;

                        float ao1 = 1.0f; //neg x pos z
                        float ao2 = 1.0f; //pos x pos z
                        float ao3 = 1.0f; //neg x neg z
                        float ao4 = 1.0f; //pos x neg z

                        if (getBlockFromWorld(x - 1, y - 1, z) != 0 || getBlockFromWorld(x - 1, y - 1, z + 1) != 0 || getBlockFromWorld(x, y - 1, z + 1) != 0)
                            ao1 = 0.8f;
                        if (getBlockFromWorld(x + 1, y - 1, z) != 0 || getBlockFromWorld(x + 1, y - 1, z + 1) != 0 || getBlockFromWorld(x, y - 1, z + 1) != 0)
                            ao2 = 0.8f;
                        if (getBlockFromWorld(x - 1, y - 1, z) != 0 || getBlockFromWorld(x - 1, y - 1, z - 1) != 0 || getBlockFromWorld(x, y - 1, z - 1) != 0)
                            ao3 = 0.8f;
                        if (getBlockFromWorld(x + 1, y - 1, z) != 0 || getBlockFromWorld(x + 1, y - 1, z - 1) != 0 || getBlockFromWorld(x, y - 1, z - 1) != 0)
                            ao4 = 0.8f;

                        ms.vertex( 0.0f+x,  0.0f+y,  0.0f+z,  0.0f, -1.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao3,  uv1.x, uv2.y);
                        ms.vertex( 1.0f+x,  0.0f+y,  0.0f+z,  0.0f, -1.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao4,  uv2.x, uv2.y);
                        ms.vertex( 1.0f+x,  0.0f+y,  1.0f+z,  0.0f, -1.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao2,  uv2.x, uv1.y);

                        ms.vertex( 1.0f+x,  0.0f+y,  1.0f+z,  0.0f, -1.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao2,  uv2.x, uv1.y);
                        ms.vertex( 0.0f+x,  0.0f+y,  1.0f+z,  0.0f, -1.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao1,  uv1.x, uv1.y);
                        ms.vertex( 0.0f+x,  0.0f+y,  0.0f+z,  0.0f, -1.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao3,  uv1.x, uv2.y);
                    }

                    if (getBlockFromWorld(x, y, z - 1) == 0 && block_X_Y_Z != 0) { //-z face
                        vec2i textureCoord = block->getTextureCoord(EnumDirection::NEGATIVE_Z);

                        vec2 uv1 = vec2((textureCoord.x / 8.0f), (textureCoord.y / 8.0f)) + bias;
                        vec2 uv2 = vec2(((textureCoord.x + 1) / 8.0f), ((textureCoord.y + 1) / 8.0f)) - bias;

                        float ao1 = 1.0f; //neg x pos y
                        float ao2 = 1.0f; //pos x pos y
                        float ao3 = 1.0f; //neg x neg y
                        float ao4 = 1.0f; //pos x neg y

                        if (getBlockFromWorld(x - 1, y, z - 1) != 0 || getBlockFromWorld(x - 1, y + 1, z - 1) != 0 || getBlockFromWorld(x, y + 1, z - 1) != 0)
                            ao1 = 0.8f;
                        if (getBlockFromWorld(x + 1, y, z - 1) != 0 || getBlockFromWorld(x + 1, y + 1, z - 1) != 0 || getBlockFromWorld(x, y + 1, z - 1) != 0)
                            ao2 = 0.8f;
                        if (getBlockFromWorld(x - 1, y, z - 1) != 0 || getBlockFromWorld(x - 1, y - 1, z - 1) != 0 || getBlockFromWorld(x, y - 1, z - 1) != 0)
                            ao3 = 0.8f;
                        if (getBlockFromWorld(x + 1, y, z - 1) != 0 || getBlockFromWorld(x + 1, y - 1, z - 1) != 0 || getBlockFromWorld(x, y - 1, z - 1) != 0)
                            ao4 = 0.8f;

                        ms.vertex( 1.0f+x,  1.0f+y,  0.0f+z,  0.0f, 0.0f, -1.0f,  blockColor.x, blockColor.y, blockColor.z, ao2,  uv2.x, uv1.y);
                        ms.vertex( 1.0f+x,  0.0f+y,  0.0f+z,  0.0f, 0.0f, -1.0f,  blockColor.x, blockColor.y, blockColor.z, ao4,  uv2.x, uv2.y);
                        ms.vertex( 0.0f+x,  0.0f+y,  0.0f+z,  0.0f, 0.0f, -1.0f,  blockColor.x, blockColor.y, blockColor.z, ao3,  uv1.x, uv2.y);

                        ms.vertex( 0.0f+x,  0.0f+y,  0.0f+z,  0.0f, 0.0f, -1.0f,  blockColor.x, blockColor.y, blockColor.z, ao3,  uv1.x, uv2.y);
                        ms.vertex( 0.0f+x,  1.0f+y,  0.0f+z,  0.0f, 0.0f, -1.0f,  blockColor.x, blockColor.y, blockColor.z, ao1,  uv1.x, uv1.y);
                        ms.vertex( 1.0f+x,  1.0f+y,  0.0f+z,  0.0f, 0.0f, -1.0f,  blockColor.x, blockColor.y, blockColor.z, ao2,  uv2.x, uv1.y);
                    }
                    if (getBlockFromWorld(x, y, z + 1) == 0 && block_X_Y_Z != 0) { //+z face
                        vec2i textureCoord = block->getTextureCoord(EnumDirection::POSITIVE_Z);

                        vec2 uv1 = vec2((textureCoord.x / 8.0f), (textureCoord.y / 8.0f)) + bias;
                        vec2 uv2 = vec2(((textureCoord.x + 1) / 8.0f), ((textureCoord.y + 1) / 8.0f)) - bias;

                        float ao1 = 1.0f; //neg x pos y
                        float ao2 = 1.0f; //pos x pos y
                        float ao3 = 1.0f; //neg x neg y
                        float ao4 = 1.0f; //pos x neg y

                        if (getBlockFromWorld(x - 1, y, z + 1) != 0 || getBlockFromWorld(x - 1, y + 1, z + 1) != 0 || getBlockFromWorld(x, y + 1, z + 1) != 0)
                            ao1 = 0.8f;
                        if (getBlockFromWorld(x + 1, y, z + 1) != 0 || getBlockFromWorld(x + 1, y + 1, z + 1) != 0 || getBlockFromWorld(x, y + 1, z + 1) != 0)
                            ao2 = 0.8f;
                        if (getBlockFromWorld(x - 1, y, z + 1) != 0 || getBlockFromWorld(x - 1, y - 1, z + 1) != 0 || getBlockFromWorld(x, y - 1, z + 1) != 0)
                            ao3 = 0.8f;
                        if (getBlockFromWorld(x + 1, y, z + 1) != 0 || getBlockFromWorld(x + 1, y - 1, z + 1) != 0 || getBlockFromWorld(x, y - 1, z + 1) != 0)
                            ao4 = 0.8f;

                        ms.vertex( 0.0f+x,  0.0f+y,  1.0f+z,  0.0f, 0.0f, 1.0f,  blockColor.x, blockColor.y, blockColor.z, ao3,  uv1.x, uv2.y);
                        ms.vertex( 1.0f+x,  0.0f+y,  1.0f+z,  0.0f, 0.0f, 1.0f,  blockColor.x, blockColor.y, blockColor.z, ao4,  uv2.x, uv2.y);
                        ms.vertex( 1.0f+x,  1.0f+y,  1.0f+z,  0.0f, 0.0f, 1.0f,  blockColor.x, blockColor.y, blockColor.z, ao2,  uv2.x, uv1.y);

                        ms.vertex( 1.0f+x,  1.0f+y,  1.0f+z,  0.0f, 0.0f, 1.0f,  blockColor.x, blockColor.y, blockColor.z, ao2,  uv2.x, uv1.y);
                        ms.vertex( 0.0f+x,  1.0f+y,  1.0f+z,  0.0f, 0.0f, 1.0f,  blockColor.x, blockColor.y, blockColor.z, ao1,  uv1.x, uv1.y);
                        ms.vertex( 0.0f+x,  0.0f+y,  1.0f+z,  0.0f, 0.0f, 1.0f,  blockColor.x, blockColor.y, blockColor.z, ao3,  uv1.x, uv2.y);
                    }
                }
            }
        }
        ms.toMesh(this->mesh);
		ms.clear();
    }
}
