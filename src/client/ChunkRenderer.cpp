#include "client/ChunkRenderer.hpp"
#include "common/Chunk.hpp"
#include "common/Block.hpp"
#include "client/WorldRenderer.hpp"

#include "rendering/Renderer.hpp"

#include <queue>

ChunkRenderer::ChunkRenderer(int chunk_x, int chunk_z) {
    this->chunk_x = chunk_x;
    this->chunk_z = chunk_z;

    this->transform = Transform(vec3(chunk_x * 16, 0.0f, chunk_z * 16), quaternion(), vec3(1.0f));
}

const float textureSize = 16.0f;
const float AO = 0.3333f;

void ChunkRenderer::generateMesh(ChunkNeighborhood &neighborhood) {
    float bias = 0.00001f;

    int highestY = 0;

    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            for (int y = 0; y < 256; y++) {

                Block &block_X_Y_Z = neighborhood.getBlock(x, y, z);


                if (block_X_Y_Z.getID() == 0) {
                    continue;
                }

                if (y > highestY) {
                    highestY = y;
                }


                bool block_X_posY_Z = !neighborhood.getBlock(x, y + 1, z).isSolid();
                bool block_X_negY_Z = !neighborhood.getBlock(x, y - 1, z).isSolid();

                bool block_X_Y_negZ = !neighborhood.getBlock(x, y, z - 1).isSolid();
                bool block_X_posY_negZ = !neighborhood.getBlock(x, y + 1, z - 1).isSolid();
                bool block_X_negY_negZ = !neighborhood.getBlock(x, y - 1, z - 1).isSolid();

                bool block_X_Y_posZ = !neighborhood.getBlock(x, y, z + 1).isSolid();
                bool block_X_posY_posZ = !neighborhood.getBlock(x, y + 1, z + 1).isSolid();
                bool block_X_negY_posZ = !neighborhood.getBlock(x, y - 1, z + 1).isSolid();


                bool block_posX_Y_Z = !neighborhood.getBlock(x + 1, y, z).isSolid();
                bool block_posX_posY_Z = !neighborhood.getBlock(x + 1, y + 1, z).isSolid();
                bool block_posX_negY_Z = !neighborhood.getBlock(x + 1, y - 1, z).isSolid();

                bool block_posX_Y_negZ = !neighborhood.getBlock(x + 1, y, z - 1).isSolid();
                bool block_posX_posY_negZ = !neighborhood.getBlock(x + 1, y + 1, z - 1).isSolid();
                bool block_posX_negY_negZ = !neighborhood.getBlock(x + 1, y - 1, z - 1).isSolid();

                bool block_posX_Y_posZ = !neighborhood.getBlock(x + 1, y, z + 1).isSolid();
                bool block_posX_posY_posZ = !neighborhood.getBlock(x + 1, y + 1, z + 1).isSolid();
                bool block_posX_negY_posZ = !neighborhood.getBlock(x + 1, y - 1, z + 1).isSolid();


                bool block_negX_Y_Z = !neighborhood.getBlock(x - 1, y, z).isSolid();
                bool block_negX_posY_Z = !neighborhood.getBlock(x - 1, y + 1, z).isSolid();
                bool block_negX_negY_Z = !neighborhood.getBlock(x - 1, y - 1, z).isSolid();

                bool block_negX_Y_negZ = !neighborhood.getBlock(x - 1, y, z - 1).isSolid();
                bool block_negX_posY_negZ = !neighborhood.getBlock(x - 1, y + 1, z - 1).isSolid();
                bool block_negX_negY_negZ = !neighborhood.getBlock(x - 1, y - 1, z - 1).isSolid();

                bool block_negX_Y_posZ = !neighborhood.getBlock(x - 1, y, z + 1).isSolid();
                bool block_negX_posY_posZ = !neighborhood.getBlock(x - 1, y + 1, z + 1).isSolid();
                bool block_negX_negY_posZ = !neighborhood.getBlock(x - 1, y - 1, z + 1).isSolid();

                if (block_posX_Y_Z) { //+x face
                    vec2i textureCoord = block_X_Y_Z.getTextureCoord(EnumDirection::POSITIVE_X);

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

                    float lightLevel = neighborhood.getSunlight(x+1, y, z) / 15.0f;
                    lightLevel += neighborhood.getTorchlight(x+1, y, z) / 15.0f;

                    mesh.pushFace(1+x, 0+y, 0+z,  uv1.x, uv2.y,  (uint8_t)(ao100*lightLevel*255.0f),  0,
                    1+x, 1+y, 0+z,  uv1.x, uv1.y,  (uint8_t)(ao110*lightLevel*255.0f),  0,
                    1+x, 1+y, 1+z,  uv2.x, uv1.y,  (uint8_t)(ao111*lightLevel*255.0f),  0,
                    1+x, 0+y, 1+z,  uv2.x, uv2.y,  (uint8_t)(ao101*lightLevel*255.0f),  0);
                }
                if (block_negX_Y_Z) { //-x face
                    vec2i textureCoord = block_X_Y_Z.getTextureCoord(EnumDirection::NEGATIVE_X);

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

                    float lightLevel = neighborhood.getSunlight(x-1, y, z) / 15.0f;
                    lightLevel += neighborhood.getTorchlight(x-1, y, z) / 15.0f;

                    mesh.pushFace( 0+x, 1+y, 1+z,  uv1.x, uv1.y,  (uint8_t)(ao011*lightLevel*255.0f),  1,
                    0+x, 1+y, 0+z,  uv2.x, uv1.y,  (uint8_t)(ao010*lightLevel*255.0f),  1,
                    0+x, 0+y, 0+z,  uv2.x, uv2.y,  (uint8_t)(ao000*lightLevel*255.0f),  1,
                    0+x, 0+y, 1+z,  uv1.x, uv2.y,  (uint8_t)(ao001*lightLevel*255.0f),  1);
                }

                if (block_X_posY_Z) { //top face
                    vec2i textureCoord = block_X_Y_Z.getTextureCoord(EnumDirection::POSITIVE_Y);

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

                    float lightLevel = neighborhood.getSunlight(x, y+1, z) / 15.0f;
                    lightLevel += neighborhood.getTorchlight(x, y+1, z) / 15.0f;

                    mesh.pushFace( 1+x, 1+y, 1+z,  uv2.x, uv1.y,  (uint8_t)(ao111*lightLevel*255.0f),  2,
                    1+x, 1+y, 0+z,  uv2.x, uv2.y,  (uint8_t)(ao110*lightLevel*255.0f),  2,
                    0+x, 1+y, 0+z,  uv1.x, uv2.y,  (uint8_t)(ao010*lightLevel*255.0f),  2,
                    0+x, 1+y, 1+z,  uv1.x, uv1.y,  (uint8_t)(ao011*lightLevel*255.0f),  2);
                }
                if (block_X_negY_Z) { //bottom face
                    vec2i textureCoord = block_X_Y_Z.getTextureCoord(EnumDirection::NEGATIVE_Y);

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

                    float lightLevel = neighborhood.getSunlight(x, y-1, z) / 15.0f;
                    lightLevel += neighborhood.getTorchlight(x, y-1, z) / 15.0f;

                    mesh.pushFace( 0+x, 0+y, 0+z,  uv1.x, uv2.y,  (uint8_t)(ao000*lightLevel*255.0f),  3,
                    1+x, 0+y, 0+z,  uv2.x, uv2.y,  (uint8_t)(ao100*lightLevel*255.0f),  3,
                    1+x, 0+y, 1+z,  uv2.x, uv1.y,  (uint8_t)(ao101*lightLevel*255.0f),  3,
                    0+x, 0+y, 1+z,  uv1.x, uv1.y,  (uint8_t)(ao001*lightLevel*255.0f),  3);
                }
                if (block_X_Y_posZ) { //+z face
                    vec2i textureCoord = block_X_Y_Z.getTextureCoord(EnumDirection::POSITIVE_Z);

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

                    float lightLevel = neighborhood.getSunlight(x, y, z+1) / 15.0f;
                    lightLevel += neighborhood.getTorchlight(x, y, z+1) / 15.0f;

                    mesh.pushFace( 0+x, 0+y, 1+z,  uv1.x, uv2.y, (uint8_t)(ao001*lightLevel*255.0f),  4,
                    1+x, 0+y, 1+z,  uv2.x, uv2.y,  (uint8_t)(ao101*lightLevel*255.0f),  4,
                    1+x, 1+y, 1+z,  uv2.x, uv1.y,  (uint8_t)(ao111*lightLevel*255.0f),  4,
                    0+x, 1+y, 1+z,  uv1.x, uv1.y,  (uint8_t)(ao011*lightLevel*255.0f),  4);
                }
                if (block_X_Y_negZ) { //-z face
                    vec2i textureCoord = block_X_Y_Z.getTextureCoord(EnumDirection::NEGATIVE_Z);

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

                    float lightLevel = neighborhood.getSunlight(x, y, z-1) / 15.0f;
                    lightLevel += neighborhood.getTorchlight(x, y, z-1) / 15.0f;

                    mesh.pushFace( 1+x, 1+y, 0+z,  uv2.x, uv1.y,  (uint8_t)(ao110*lightLevel*255.0f),  5,
                    1+x, 0+y, 0+z,  uv2.x, uv2.y,  (uint8_t)(ao100*lightLevel*255.0f),  5,
                    0+x, 0+y, 0+z,  uv1.x, uv2.y,  (uint8_t)(ao000*lightLevel*255.0f),  5,
                    0+x, 1+y, 0+z,  uv1.x, uv1.y,  (uint8_t)(ao010*lightLevel*255.0f),  5);
                }
            }
        }
    }

    aabb = AABB(vec3((chunk_x * 16), 0, (chunk_z * 16)), vec3(16 + (chunk_x * 16), highestY+1, 16 + (chunk_z * 16)));

    this->updateMesh = true;
}

void ChunkRenderer::render(Material *mat) {
    if (this->updateMesh) {
        mesh.generate();

        this->updateMesh = false;
    }

    Renderer::render(&this->mesh, mat, &transform, &aabb);
    //Renderer::debug.renderDebugAABB(aabb, vec3(0.0f, 1.0f, 0.0f));

   //Renderer::debug.renderDebugAABB(AABB(vec3(attachedChunk.chunk_x*16, 0, attachedChunk.chunk_z*16), vec3(attachedChunk.chunk_x*16, 0, attachedChunk.chunk_z*16) + vec3(16, 256, 16)), vec3(0.0f, 1.0f, 0.0f));
}