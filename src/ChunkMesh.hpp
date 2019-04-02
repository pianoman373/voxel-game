#pragma once

#include <crucible/IRenderable.hpp>
#include <crucible/Math.hpp>

#include <vector>

class ChunkMesh : public IRenderable {
private:
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;

    int length = 0;
    int bufferLength = 0;

public:
    std::vector<float> data;
    std::vector<float> buffer;

    std::vector<unsigned int> indices;
    std::vector<unsigned int> indicesBuffer;

    void pushVertex(uint16_t positionX, uint16_t positionY, uint16_t positionZ, float u, float v, uint8_t ao, uint8_t index);

    void pushFace(uint16_t positionX1, uint16_t positionY1, uint16_t positionZ1, float u1, float v1, uint8_t ao1, uint8_t index1,
                  uint16_t positionX2, uint16_t positionY2, uint16_t positionZ2, float u2, float v2, uint8_t ao2, uint8_t index2,
                  uint16_t positionX3, uint16_t positionY3, uint16_t positionZ3, float u3, float v3, uint8_t ao3, uint8_t index3,
                  uint16_t positionX4, uint16_t positionY4, uint16_t positionZ4, float u4, float v4, uint8_t ao4, uint8_t index4);

    void generate();

    void clear();

    void render() const;

    void destroy();
};