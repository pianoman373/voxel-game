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
    return 0;//world->getBlock((this->chunk_x * CHUNK_SIZE) + x, (this->chunk_y * CHUNK_SIZE) + y, (this->chunk_z * CHUNK_SIZE) + z);
}

//WIP
int Chunk::getBlockFromWorld(int x, int y, int z) {
    //return world->getBlock((this->chunk_x * CHUNK_SIZE) + x, (this->chunk_y * CHUNK_SIZE) + y, (this->chunk_z * CHUNK_SIZE) + z);
    return getBlock(x, y, z);
}

void Chunk::setBlock(int x, int y, int z, char block) {
    if (x < CHUNK_SIZE && x >= 0 && y < CHUNK_SIZE && y >= 0 && z < CHUNK_SIZE && z >= 0) {
        blocks[x][y][z] = block;

        if (block != 0) {
            if (this->empty) {
                this->empty = false;
            }
        }
    }

    this->rebuild = true;
}

const float textureSize = 16.0f;
const float AO = 0.4f;


void Chunk::generateMesh() {
    if (!empty) {
        MeshFactory ms;
        float bias = 0.00001f;

        for (int x = 0; x < CHUNK_SIZE; x++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                for (int y = 0; y < CHUNK_SIZE; y++) {
                    int block_X_Y_Z = getBlockFromWorld(x, y, z);

                    if (block_X_Y_Z == 0)
                       continue;


                    Block *block = BlockRegistry::getBlock(block_X_Y_Z);

                    vec3 blockColor = block->color;


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

                        ms.vertex(1.0f+x,  0.0f+y,  0.0f+z,  1.0f, 0.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao100,  uv1.x, uv2.y);
                        ms.vertex(1.0f+x,  1.0f+y,  0.0f+z,  1.0f, 0.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao110,  uv1.x, uv1.y);
                        ms.vertex(1.0f+x,  1.0f+y,  1.0f+z,  1.0f, 0.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao111,  uv2.x, uv1.y);

                        ms.vertex(1.0f+x,  1.0f+y,  1.0f+z,  1.0f, 0.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao111,  uv2.x, uv1.y);
                        ms.vertex(1.0f+x,  0.0f+y,  1.0f+z,  1.0f, 0.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao101,  uv2.x, uv2.y);
                        ms.vertex(1.0f+x,  0.0f+y,  0.0f+z,  1.0f, 0.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao100,  uv1.x, uv2.y);
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

                        ms.vertex( 0.0f+x,  1.0f+y,  1.0f+z,  -1.0f, 0.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao011,  uv1.x, uv1.y);
                        ms.vertex( 0.0f+x,  1.0f+y,  0.0f+z,  -1.0f, 0.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao010,  uv2.x, uv1.y);
                        ms.vertex( 0.0f+x,  0.0f+y,  0.0f+z,  -1.0f, 0.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao000,  uv2.x, uv2.y);

                        ms.vertex( 0.0f+x,  0.0f+y,  0.0f+z,  -1.0f, 0.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao000,  uv2.x, uv2.y);
                        ms.vertex( 0.0f+x,  0.0f+y,  1.0f+z,  -1.0f, 0.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao001,  uv1.x, uv2.y);
                        ms.vertex( 0.0f+x,  1.0f+y,  1.0f+z,  -1.0f, 0.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao011,  uv1.x, uv1.y);
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

                        ms.vertex( 1.0f+x,  1.0f+y,  1.0f+z,  0.0f, 1.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao111,  uv2.x, uv1.y);
                        ms.vertex( 1.0f+x,  1.0f+y,  0.0f+z,  0.0f, 1.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao110,  uv2.x, uv2.y);
                        ms.vertex( 0.0f+x,  1.0f+y,  0.0f+z,  0.0f, 1.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao010,  uv1.x, uv2.y);

                        ms.vertex( 0.0f+x,  1.0f+y,  0.0f+z,  0.0f, 1.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao010,  uv1.x, uv2.y);
                        ms.vertex( 0.0f+x,  1.0f+y,  1.0f+z,  0.0f, 1.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao011,  uv1.x, uv1.y);
                        ms.vertex( 1.0f+x,  1.0f+y,  1.0f+z,  0.0f, 1.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao111,  uv2.x, uv1.y);
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

                        ms.vertex( 0.0f+x,  0.0f+y,  0.0f+z,  0.0f, -1.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao000,  uv1.x, uv2.y);
                        ms.vertex( 1.0f+x,  0.0f+y,  0.0f+z,  0.0f, -1.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao100,  uv2.x, uv2.y);
                        ms.vertex( 1.0f+x,  0.0f+y,  1.0f+z,  0.0f, -1.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao101,  uv2.x, uv1.y);

                        ms.vertex( 1.0f+x,  0.0f+y,  1.0f+z,  0.0f, -1.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao101,  uv2.x, uv1.y);
                        ms.vertex( 0.0f+x,  0.0f+y,  1.0f+z,  0.0f, -1.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao001,  uv1.x, uv1.y);
                        ms.vertex( 0.0f+x,  0.0f+y,  0.0f+z,  0.0f, -1.0f, 0.0f,  blockColor.x, blockColor.y, blockColor.z, ao000,  uv1.x, uv2.y);
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

                        ms.vertex( 1.0f+x,  1.0f+y,  0.0f+z,  0.0f, 0.0f, -1.0f,  blockColor.x, blockColor.y, blockColor.z, ao110,  uv2.x, uv1.y);
                        ms.vertex( 1.0f+x,  0.0f+y,  0.0f+z,  0.0f, 0.0f, -1.0f,  blockColor.x, blockColor.y, blockColor.z, ao100,  uv2.x, uv2.y);
                        ms.vertex( 0.0f+x,  0.0f+y,  0.0f+z,  0.0f, 0.0f, -1.0f,  blockColor.x, blockColor.y, blockColor.z, ao000,  uv1.x, uv2.y);

                        ms.vertex( 0.0f+x,  0.0f+y,  0.0f+z,  0.0f, 0.0f, -1.0f,  blockColor.x, blockColor.y, blockColor.z, ao000,  uv1.x, uv2.y);
                        ms.vertex( 0.0f+x,  1.0f+y,  0.0f+z,  0.0f, 0.0f, -1.0f,  blockColor.x, blockColor.y, blockColor.z, ao010,  uv1.x, uv1.y);
                        ms.vertex( 1.0f+x,  1.0f+y,  0.0f+z,  0.0f, 0.0f, -1.0f,  blockColor.x, blockColor.y, blockColor.z, ao110,  uv2.x, uv1.y);
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

                        ms.vertex( 0.0f+x,  0.0f+y,  1.0f+z,  0.0f, 0.0f, 1.0f,  blockColor.x, blockColor.y, blockColor.z, ao001,  uv1.x, uv2.y);
                        ms.vertex( 1.0f+x,  0.0f+y,  1.0f+z,  0.0f, 0.0f, 1.0f,  blockColor.x, blockColor.y, blockColor.z, ao101,  uv2.x, uv2.y);
                        ms.vertex( 1.0f+x,  1.0f+y,  1.0f+z,  0.0f, 0.0f, 1.0f,  blockColor.x, blockColor.y, blockColor.z, ao111,  uv2.x, uv1.y);

                        ms.vertex( 1.0f+x,  1.0f+y,  1.0f+z,  0.0f, 0.0f, 1.0f,  blockColor.x, blockColor.y, blockColor.z, ao111,  uv2.x, uv1.y);
                        ms.vertex( 0.0f+x,  1.0f+y,  1.0f+z,  0.0f, 0.0f, 1.0f,  blockColor.x, blockColor.y, blockColor.z, ao011,  uv1.x, uv1.y);
                        ms.vertex( 0.0f+x,  0.0f+y,  1.0f+z,  0.0f, 0.0f, 1.0f,  blockColor.x, blockColor.y, blockColor.z, ao001,  uv1.x, uv2.y);
                    }
                }
            }
        }
        ms.toMesh(this->mesh);
        ms.clear();
    }

    this->rebuild = false;
}
