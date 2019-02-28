#include "ChunkRenderer.hpp"
#include "Chunk.hpp"
#include "Block.hpp"
#include "WorldRenderer.hpp"

#include <crucible/Renderer.hpp>

ChunkRenderer::ChunkRenderer(Chunk &attachedChunk): attachedChunk(attachedChunk) {
    this->transform = Transform(vec3(attachedChunk.chunk_x * 16, 0.0f, attachedChunk.chunk_z * 16), quaternion(), vec3(1.0f));
}

const float textureSize = 16.0f;
const float AO = 0.3333f;

char ChunkRenderer::getBlock(ChunkRemeshJob &job, int x, int y, int z) {

    //center
    if (x >= 0 && x < 16 && z >= 0 && z < 16) {
        return attachedChunk.getBlock(x, y, z);
    }

    //positive X
    if (x >= 16 && z >= 0 && z < 16) {
        return job.posX->getBlock(x - 16, y, z);
    }
    //negative X
    if (x < 0 && z >= 0 && z < 16) {
        return job.negX->getBlock(x + 16, y, z);
    }

    //positive Z
    if (x >= 0 && x < 16 && z >= 16) {
        return job.posZ->getBlock(x, y, z - 16);
    }
    //negative Z
    if (x >= 0 && x < 16 && z < 0) {
        return job.negZ->getBlock(x, y, z + 16);
    }


    //positive X positive Z
    if (x >= 16 &&  z >= 16) {
        return job.posXposZ->getBlock(x - 16, y, z - 16);
    }
    //positive X negative Z
    if (x >= 16 && z < 0) {
        return job.posXnegZ->getBlock(x - 16, y, z + 16);
    }

    //negative X positive Z
    if (x < 0 &&  z >= 16) {
        return job.negXposZ->getBlock(x + 16, y, z - 16);
    }
    //negative X negative Z
    if (x < 0 && z < 0) {
        return job.negXnegZ->getBlock(x + 16, y, z + 16);
    }

    return 1;
}

void ChunkRenderer::generateMesh(ChunkRemeshJob &job) {
    if (!attachedChunk.empty) {
        float bias = 0.00001f;

        vec3i min = vec3i(16, 256, 16);
        vec3i max = vec3i(0, 0, 0);

        for (int x = 0; x < 16; x++) {
            for (int z = 0; z < 16; z++) {
                for (int y = 0; y < 256; y++) {

                    int block_X_Y_Z = getBlock(job, x, y, z);


                    if (block_X_Y_Z == 0) {
                        continue;
                    }
                    else {
                        if (x < min.x) {
                            min.x = x;
                        }


                        if (y < min.y) {
                            min.y = y;
                        }


                        if (z < min.z) {
                            min.z = z;
                        }




                        if (x > max.x) {
                            max.x = x;
                        }


                        if (y > max.y) {
                            max.y = y;
                        }


                        if (z > max.z) {
                            max.z = z;
                        }
                    }


                    float lightLevel = 0.0f;


                    Block *block = BlockRegistry::getBlock(block_X_Y_Z);


                    bool block_X_posY_Z = !BlockRegistry::getBlock(getBlock(job, x, y + 1, z))->isSolid();
                    bool block_X_negY_Z = !BlockRegistry::getBlock(getBlock(job, x, y - 1, z))->isSolid();

                    bool block_X_Y_negZ = !BlockRegistry::getBlock(getBlock(job, x, y, z - 1))->isSolid();
                    bool block_X_posY_negZ = !BlockRegistry::getBlock(getBlock(job, x, y + 1, z - 1))->isSolid();
                    bool block_X_negY_negZ = !BlockRegistry::getBlock(getBlock(job, x, y - 1, z - 1))->isSolid();

                    bool block_X_Y_posZ = !BlockRegistry::getBlock(getBlock(job, x, y, z + 1))->isSolid();
                    bool block_X_posY_posZ = !BlockRegistry::getBlock(getBlock(job, x, y + 1, z + 1))->isSolid();
                    bool block_X_negY_posZ = !BlockRegistry::getBlock(getBlock(job, x, y - 1, z + 1))->isSolid();


                    bool block_posX_Y_Z = !BlockRegistry::getBlock(getBlock(job, x + 1, y, z))->isSolid();
                    bool block_posX_posY_Z = !BlockRegistry::getBlock(getBlock(job, x + 1, y + 1, z))->isSolid();
                    bool block_posX_negY_Z = !BlockRegistry::getBlock(getBlock(job, x + 1, y - 1, z))->isSolid();

                    bool block_posX_Y_negZ = !BlockRegistry::getBlock(getBlock(job, x + 1, y, z - 1))->isSolid();
                    bool block_posX_posY_negZ = !BlockRegistry::getBlock(getBlock(job, x + 1, y + 1, z - 1))->isSolid();
                    bool block_posX_negY_negZ = !BlockRegistry::getBlock(getBlock(job, x + 1, y - 1, z - 1))->isSolid();

                    bool block_posX_Y_posZ = !BlockRegistry::getBlock(getBlock(job, x + 1, y, z + 1))->isSolid();
                    bool block_posX_posY_posZ = !BlockRegistry::getBlock(getBlock(job, x + 1, y + 1, z + 1))->isSolid();
                    bool block_posX_negY_posZ = !BlockRegistry::getBlock(getBlock(job, x + 1, y - 1, z + 1))->isSolid();


                    bool block_negX_Y_Z = !BlockRegistry::getBlock(getBlock(job, x - 1, y, z))->isSolid();
                    bool block_negX_posY_Z = !BlockRegistry::getBlock(getBlock(job, x - 1, y + 1, z))->isSolid();
                    bool block_negX_negY_Z = !BlockRegistry::getBlock(getBlock(job, x - 1, y - 1, z))->isSolid();

                    bool block_negX_Y_negZ = !BlockRegistry::getBlock(getBlock(job, x - 1, y, z - 1))->isSolid();
                    bool block_negX_posY_negZ = !BlockRegistry::getBlock(getBlock(job, x - 1, y + 1, z - 1))->isSolid();
                    bool block_negX_negY_negZ = !BlockRegistry::getBlock(getBlock(job, x - 1, y - 1, z - 1))->isSolid();

                    bool block_negX_Y_posZ = !BlockRegistry::getBlock(getBlock(job, x - 1, y, z + 1))->isSolid();
                    bool block_negX_posY_posZ = !BlockRegistry::getBlock(getBlock(job, x - 1, y + 1, z + 1))->isSolid();
                    bool block_negX_negY_posZ = !BlockRegistry::getBlock(getBlock(job, x - 1, y - 1, z + 1))->isSolid();

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

                        vec3 blockColor = vec3(1.0f);

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

                        vec3 blockColor = vec3(1.0f);

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

                        float lightLevel = 1.0f;

                        mesh.pushVertex( 1.0f+x, 1.0f+y, 1.0f+z,  uv2.x, uv1.y,  ao111*lightLevel,  2);
                        mesh.pushVertex( 1.0f+x, 1.0f+y, 0.0f+z,  uv2.x, uv2.y,  ao110*lightLevel,  2);
                        mesh.pushVertex( 0.0f+x, 1.0f+y, 0.0f+z,  uv1.x, uv2.y,  ao010*lightLevel,  2);

                        mesh.pushVertex( 0.0f+x, 1.0f+y, 0.0f+z,  uv1.x, uv2.y,  ao010*lightLevel,  2);
                        mesh.pushVertex( 0.0f+x, 1.0f+y, 1.0f+z,  uv1.x, uv1.y,  ao011*lightLevel,  2);
                        mesh.pushVertex( 1.0f+x, 1.0f+y, 1.0f+z,  uv2.x, uv1.y,  ao111*lightLevel,  2);
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

                        vec3 blockColor = vec3(1.0f);

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

                        vec3 blockColor = vec3(1.0f);

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

                        vec3 blockColor = vec3(1.0f);

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

        aabb = AABB(vec3(min.x + (attachedChunk.chunk_x * 16), min.y, min.z + (attachedChunk.chunk_z * 16)), vec3(max.x+1 + (attachedChunk.chunk_x * 16), max.y+1, max.z+1 + (attachedChunk.chunk_z * 16)));
    }

    this->rebuild = true;
}

void ChunkRenderer::render(Material *mat) {
    if (this->rebuild) {
        mesh.generate();
        //mesh.clear();

        this->rebuild = false;
    }

    if (this->mesh.data.size() > 0) {
        Renderer::render(this->mesh, *mat, transform, aabb);
        //Renderer::render(Renderer::cubemapMesh, *mat, transform, aabb);
        //Renderer::debug.renderDebugAABB(aabb, vec3(1.0f, 0.0f, 0.0f));
    }

   //Renderer::debug.renderDebugAABB(AABB(vec3(attachedChunk.chunk_x*16, 0, attachedChunk.chunk_z*16), vec3(attachedChunk.chunk_x*16, 0, attachedChunk.chunk_z*16) + vec3(16, 256, 16)), vec3(0.0f, 1.0f, 0.0f));
}