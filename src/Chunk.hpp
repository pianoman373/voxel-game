#pragma once

#define CHUNK_SIZE 32

#include "Mesh.hpp"
#include "Shader.hpp"

class Chunk {
private:
    char blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
    Mesh mesh;

public:
    int chunk_x;
    int chunk_y;
    int chunk_z;

    Chunk(int x, int y, int z);

    ~Chunk();

    char getBlock(int x, int y, int z);

    void setBlock(int x, int y, int z, char block);

    void generateMesh();

    void render(Shader &shader);
};
