#include "Chunk.hpp"
#include "MeshFactory.hpp"

Chunk::Chunk(int x, int y, int z) {
    chunk_x = x;
    chunk_y = y;
    chunk_z = z;
}

Chunk::~Chunk() {
    delete mesh;
}

char Chunk::getBlock(int x, int y, int z) {
    if (x < CHUNK_SIZE && x >= 0 && y < CHUNK_SIZE && y >= 0 && z < CHUNK_SIZE && z >= 0) {
        return blocks[x][y][z];
    }
    return 0;
}

void Chunk::setBlock(int x, int y, int z, char block) {
    if (x < CHUNK_SIZE && x >= 0 && y < CHUNK_SIZE && y >= 0 && z < CHUNK_SIZE && z >= 0) {
        blocks[x][y][z] = block;
    }
}

void Chunk::generateMesh() {
    MeshFactory ms = MeshFactory();

    for (int x = 0; x < 64; x++) {
        for (int z = 0; z < 64; z++) {
            for (int y = 0; y < 64; y++) {
                if (getBlock(x + 1, y, z) == 0 && getBlock(x, y, z) == 1) {
                    ms.vertex(0.5f+x, -0.5f+y, -0.5f+z,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f);
                    ms.vertex(0.5f+x,  0.5f+y, -0.5f+z,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f);
                    ms.vertex(0.5f+x,  0.5f+y,  0.5f+z,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f);

                    ms.vertex(0.5f+x,  0.5f+y,  0.5f+z,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f);
                    ms.vertex(0.5f+x, -0.5f+y,  0.5f+z,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f);
                    ms.vertex(0.5f+x, -0.5f+y, -0.5f+z,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f);
                }
                if (getBlock(x - 1, y, z) == 0 && getBlock(x, y, z) == 1) {
                    ms.vertex(-0.5f+x,  0.5f+y,  0.5f+z,  -1.0f, 0.0f, 0.0f,  1.0f, 0.0f);
                    ms.vertex(-0.5f+x,  0.5f+y, -0.5f+z,  -1.0f, 0.0f, 0.0f,  1.0f, 1.0f);
                    ms.vertex(-0.5f+x, -0.5f+y, -0.5f+z,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f);

                    ms.vertex(-0.5f+x, -0.5f+y, -0.5f+z,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f);
                    ms.vertex(-0.5f+x, -0.5f+y,  0.5f+z,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f);
                    ms.vertex(-0.5f+x,  0.5f+y,  0.5f+z,  -1.0f, 0.0f, 0.0f,  1.0f, 0.0f);
                }

                if (getBlock(x, y + 1, z) == 0 && getBlock(x, y, z) == 1) {
                    ms.vertex( 0.5f+x,  0.5f+y,  0.5f+z,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f);
                    ms.vertex( 0.5f+x,  0.5f+y, -0.5f+z,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f);
                    ms.vertex(-0.5f+x,  0.5f+y, -0.5f+z,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f);

                    ms.vertex(-0.5f+x,  0.5f+y, -0.5f+z,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f);
                    ms.vertex(-0.5f+x,  0.5f+y,  0.5f+z,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f);
                    ms.vertex( 0.5f+x,  0.5f+y,  0.5f+z,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f);
                }
                if (getBlock(x, y - 1, z) == 0 && getBlock(x, y, z) == 1) {
                    ms.vertex(-0.5f+x, -0.5f+y, -0.5f+z,  0.0f, -1.0f, 0.0f,  0.0f, 1.0f);
                    ms.vertex( 0.5f+x, -0.5f+y, -0.5f+z,  0.0f, -1.0f, 0.0f,  1.0f, 1.0f);
                    ms.vertex( 0.5f+x, -0.5f+y,  0.5f+z,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f);

                    ms.vertex( 0.5f+x, -0.5f+y,  0.5f+z,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f);
                    ms.vertex(-0.5f+x, -0.5f+y,  0.5f+z,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f);
                    ms.vertex(-0.5f+x, -0.5f+y, -0.5f+z,  0.0f, -1.0f, 0.0f,  0.0f, 1.0f);
                }

                if (getBlock(x, y, z - 1) == 0 && getBlock(x, y, z) == 1) {
                    ms.vertex( 0.5f+x,  0.5f+y, -0.5f+z,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f);
                    ms.vertex( 0.5f+x, -0.5f+y, -0.5f+z,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f);
                    ms.vertex(-0.5f+x, -0.5f+y, -0.5f+z,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f);

                    ms.vertex(-0.5f+x, -0.5f+y, -0.5f+z,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f);
                    ms.vertex(-0.5f+x,  0.5f+y, -0.5f+z,  0.0f, 0.0f, -1.0f,  0.0f, 1.0f);
                    ms.vertex( 0.5f+x,  0.5f+y, -0.5f+z,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f);
                }
                if (getBlock(x, y, z + 1) == 0 && getBlock(x, y, z) == 1) {
                    ms.vertex(-0.5f+x, -0.5f+y,  0.5f+z,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f);
                    ms.vertex( 0.5f+x, -0.5f+y,  0.5f+z,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f);
                    ms.vertex( 0.5f+x,  0.5f+y,  0.5f+z,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f);

                    ms.vertex( 0.5f+x,  0.5f+y,  0.5f+z,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f);
                    ms.vertex(-0.5f+x,  0.5f+y,  0.5f+z,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f);
                    ms.vertex(-0.5f+x, -0.5f+y,  0.5f+z,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f);
                }
            }
        }
    }
    this->mesh = ms.toMesh();
}

void Chunk::render(Shader *shader) {
    glm::mat4 model = glm::mat4();
    shader->uniform("model", glm::translate(model, glm::vec3(chunk_x * 64, chunk_y * 64, chunk_z * 64)));
    this->mesh->render();
}
