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

    void pushVertex(float positionX, float positionY, float positionZ, float u, float v, float ao, unsigned int index);

    void generate();

    void clear();

    void render();

    void destroy();
};