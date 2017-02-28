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
                float r = 1.0f;
                float g = 1.0f;
                float b = 1.0f;

                if (getBlock(x + 1, y, z) == 0 && getBlock(x, y, z) != 0) { //+x face
                    glm::vec3 col1 = glm::vec3(1.0f); //neg y pos z
                    glm::vec3 col2 = glm::vec3(1.0f); //pos y pos z
                    glm::vec3 col3 = glm::vec3(1.0f); //neg y neg z
                    glm::vec3 col4 = glm::vec3(1.0f); //pos y neg z

                    if (getBlock(x + 1, y - 1, z) != 0 || getBlock(x + 1, y - 1, z + 1) != 0 || getBlock(x + 1, y, z + 1) != 0)
                        col1 = glm::vec3(0.8f);
                    if (getBlock(x + 1, y + 1, z) != 0 || getBlock(x + 1, y + 1, z + 1) != 0 || getBlock(x + 1, y, z + 1) != 0)
                        col2 = glm::vec3(0.8f);
                    if (getBlock(x + 1, y - 1, z) != 0 || getBlock(x + 1, y - 1, z - 1) != 0 || getBlock(x + 1, y, z - 1) != 0)
                        col3 = glm::vec3(0.8f);
                    if (getBlock(x + 1, y + 1, z) != 0 || getBlock(x + 1, y + 1, z - 1) != 0 || getBlock(x + 1, y, z - 1) != 0)
                        col4 = glm::vec3(0.8f);

                    ms.vertex(1.0f+x,  0.0f+y,  0.0f+z,  1.0f, 0.0f, 0.0f,  col3.x, col3.y, col3.z,  0.0f, 1.0f);
                    ms.vertex(1.0f+x,  1.0f+y,  0.0f+z,  1.0f, 0.0f, 0.0f,  col4.x, col4.y, col4.z,  1.0f, 1.0f);
                    ms.vertex(1.0f+x,  1.0f+y,  1.0f+z,  1.0f, 0.0f, 0.0f,  col2.x, col2.y, col2.z,  1.0f, 0.0f);

                    ms.vertex(1.0f+x,  1.0f+y,  1.0f+z,  1.0f, 0.0f, 0.0f,  col2.x, col2.y, col2.z,  1.0f, 0.0f);
                    ms.vertex(1.0f+x,  0.0f+y,  1.0f+z,  1.0f, 0.0f, 0.0f,  col1.x, col1.y, col1.z,  0.0f, 0.0f);
                    ms.vertex(1.0f+x,  0.0f+y,  0.0f+z,  1.0f, 0.0f, 0.0f,  col3.x, col3.y, col3.z,  0.0f, 1.0f);
                }
                if (getBlock(x - 1, y, z) == 0 && getBlock(x, y, z) != 0) { //-x face
                    glm::vec3 col1 = glm::vec3(1.0f); //neg y pos z
                    glm::vec3 col2 = glm::vec3(1.0f); //pos y pos z
                    glm::vec3 col3 = glm::vec3(1.0f); //neg y neg z
                    glm::vec3 col4 = glm::vec3(1.0f); //pos y neg z

                    if (getBlock(x - 1, y - 1, z) != 0 || getBlock(x - 1, y - 1, z + 1) != 0 || getBlock(x - 1, y, z + 1) != 0)
                        col1 = glm::vec3(0.8f);
                    if (getBlock(x - 1, y + 1, z) != 0 || getBlock(x - 1, y + 1, z + 1) != 0 || getBlock(x - 1, y, z + 1) != 0)
                        col2 = glm::vec3(0.8f);
                    if (getBlock(x - 1, y - 1, z) != 0 || getBlock(x - 1, y - 1, z - 1) != 0 || getBlock(x - 1, y, z - 1) != 0)
                        col3 = glm::vec3(0.8f);
                    if (getBlock(x - 1, y + 1, z) != 0 || getBlock(x - 1, y + 1, z - 1) != 0 || getBlock(x - 1, y, z - 1) != 0)
                        col4 = glm::vec3(0.8f);

                    ms.vertex( 0.0f+x,  1.0f+y,  1.0f+z,  -1.0f, 0.0f, 0.0f,  col2.x, col2.y, col2.z,  1.0f, 0.0f);
                    ms.vertex( 0.0f+x,  1.0f+y,  0.0f+z,  -1.0f, 0.0f, 0.0f,  col4.x, col4.y, col4.z,  1.0f, 1.0f);
                    ms.vertex( 0.0f+x,  0.0f+y,  0.0f+z,  -1.0f, 0.0f, 0.0f,  col3.x, col3.y, col3.z,  0.0f, 1.0f);

                    ms.vertex( 0.0f+x,  0.0f+y,  0.0f+z,  -1.0f, 0.0f, 0.0f,  col3.x, col3.y, col3.z,  0.0f, 1.0f);
                    ms.vertex( 0.0f+x,  0.0f+y,  1.0f+z,  -1.0f, 0.0f, 0.0f,  col1.x, col1.y, col1.z,  0.0f, 0.0f);
                    ms.vertex( 0.0f+x,  1.0f+y,  1.0f+z,  -1.0f, 0.0f, 0.0f,  col2.x, col2.y, col2.z,  1.0f, 0.0f);
                }

                if (getBlock(x, y + 1, z) == 0 && getBlock(x, y, z) != 0) { //top face
                    glm::vec3 col1 = glm::vec3(1.0f); //neg x pos z
                    glm::vec3 col2 = glm::vec3(1.0f); //pos x pos z
                    glm::vec3 col3 = glm::vec3(1.0f); //neg x neg z
                    glm::vec3 col4 = glm::vec3(1.0f); //pos x neg z

                    if (getBlock(x - 1, y + 1, z) != 0 || getBlock(x - 1, y + 1, z + 1) != 0 || getBlock(x, y + 1, z + 1) != 0)
                        col1 = glm::vec3(0.8f);
                    if (getBlock(x + 1, y + 1, z) != 0 || getBlock(x + 1, y + 1, z + 1) != 0 || getBlock(x, y + 1, z + 1) != 0)
                        col2 = glm::vec3(0.8f);
                    if (getBlock(x - 1, y + 1, z) != 0 || getBlock(x - 1, y + 1, z - 1) != 0 || getBlock(x, y + 1, z - 1) != 0)
                        col3 = glm::vec3(0.8f);
                    if (getBlock(x + 1, y + 1, z) != 0 || getBlock(x + 1, y + 1, z - 1) != 0 || getBlock(x, y + 1, z - 1) != 0)
                        col4 = glm::vec3(0.8f);

                    ms.vertex( 1.0f+x,  1.0f+y,  1.0f+z,  0.0f, 1.0f, 0.0f,  col2.x, col2.y, col2.z,  1.0f, 0.0f);
                    ms.vertex( 1.0f+x,  1.0f+y,  0.0f+z,  0.0f, 1.0f, 0.0f,  col4.x, col4.y, col4.z,  1.0f, 1.0f);
                    ms.vertex( 0.0f+x,  1.0f+y,  0.0f+z,  0.0f, 1.0f, 0.0f,  col3.x, col3.y, col3.z,  0.0f, 1.0f);

                    ms.vertex( 0.0f+x,  1.0f+y,  0.0f+z,  0.0f, 1.0f, 0.0f,  col3.x, col3.y, col3.z,  0.0f, 1.0f);
                    ms.vertex( 0.0f+x,  1.0f+y,  1.0f+z,  0.0f, 1.0f, 0.0f,  col1.x, col1.y, col1.z,  0.0f, 0.0f);
                    ms.vertex( 1.0f+x,  1.0f+y,  1.0f+z,  0.0f, 1.0f, 0.0f,  col2.x, col2.y, col2.z,  1.0f, 0.0f);
                }
                if (getBlock(x, y - 1, z) == 0 && getBlock(x, y, z) != 0) { //bottom face
                    glm::vec3 col1 = glm::vec3(1.0f); //neg x pos z
                    glm::vec3 col2 = glm::vec3(1.0f); //pos x pos z
                    glm::vec3 col3 = glm::vec3(1.0f); //neg x neg z
                    glm::vec3 col4 = glm::vec3(1.0f); //pos x neg z

                    if (getBlock(x - 1, y - 1, z) != 0 || getBlock(x - 1, y - 1, z + 1) != 0 || getBlock(x, y - 1, z + 1) != 0)
                        col1 = glm::vec3(0.8f);
                    if (getBlock(x + 1, y - 1, z) != 0 || getBlock(x + 1, y - 1, z + 1) != 0 || getBlock(x, y - 1, z + 1) != 0)
                        col2 = glm::vec3(0.8f);
                    if (getBlock(x - 1, y - 1, z) != 0 || getBlock(x - 1, y - 1, z - 1) != 0 || getBlock(x, y - 1, z - 1) != 0)
                        col3 = glm::vec3(0.8f);
                    if (getBlock(x + 1, y - 1, z) != 0 || getBlock(x + 1, y - 1, z - 1) != 0 || getBlock(x, y - 1, z - 1) != 0)
                        col4 = glm::vec3(0.8f);

                    ms.vertex( 0.0f+x,  0.0f+y,  0.0f+z,  0.0f, -1.0f, 0.0f,  col3.x, col3.y, col3.z,  0.0f, 1.0f);
                    ms.vertex( 1.0f+x,  0.0f+y,  0.0f+z,  0.0f, -1.0f, 0.0f,  col4.x, col4.y, col4.z,  1.0f, 1.0f);
                    ms.vertex( 1.0f+x,  0.0f+y,  1.0f+z,  0.0f, -1.0f, 0.0f,  col2.x, col2.y, col2.z,  1.0f, 0.0f);

                    ms.vertex( 1.0f+x,  0.0f+y,  1.0f+z,  0.0f, -1.0f, 0.0f,  col2.x, col2.y, col2.z,  1.0f, 0.0f);
                    ms.vertex( 0.0f+x,  0.0f+y,  1.0f+z,  0.0f, -1.0f, 0.0f,  col1.x, col1.y, col1.z,  0.0f, 0.0f);
                    ms.vertex( 0.0f+x,  0.0f+y,  0.0f+z,  0.0f, -1.0f, 0.0f,  col3.x, col3.y, col3.z,  0.0f, 1.0f);
                }

                if (getBlock(x, y, z - 1) == 0 && getBlock(x, y, z) != 0) { //-z face
                    glm::vec3 col1 = glm::vec3(1.0f); //neg x pos y
                    glm::vec3 col2 = glm::vec3(1.0f); //pos x pos y
                    glm::vec3 col3 = glm::vec3(1.0f); //neg x neg y
                    glm::vec3 col4 = glm::vec3(1.0f); //pos x neg y

                    if (getBlock(x - 1, y, z - 1) != 0 || getBlock(x - 1, y + 1, z - 1) != 0 || getBlock(x, y + 1, z - 1) != 0)
                        col1 = glm::vec3(0.8f);
                    if (getBlock(x + 1, y, z - 1) != 0 || getBlock(x + 1, y + 1, z - 1) != 0 || getBlock(x, y + 1, z - 1) != 0)
                        col2 = glm::vec3(0.8f);
                    if (getBlock(x - 1, y, z - 1) != 0 || getBlock(x - 1, y - 1, z - 1) != 0 || getBlock(x, y - 1, z - 1) != 0)
                        col3 = glm::vec3(0.8f);
                    if (getBlock(x + 1, y, z - 1) != 0 || getBlock(x + 1, y - 1, z - 1) != 0 || getBlock(x, y - 1, z - 1) != 0)
                        col4 = glm::vec3(0.8f);

                    ms.vertex( 1.0f+x,  1.0f+y,  0.0f+z,  0.0f, 0.0f, -1.0f,  col2.x, col2.y, col2.z,  1.0f, 1.0f);
                    ms.vertex( 1.0f+x,  0.0f+y,  0.0f+z,  0.0f, 0.0f, -1.0f,  col4.x, col4.y, col4.z,  1.0f, 0.0f);
                    ms.vertex( 0.0f+x,  0.0f+y,  0.0f+z,  0.0f, 0.0f, -1.0f,  col3.x, col3.y, col3.z,  0.0f, 0.0f);

                    ms.vertex( 0.0f+x,  0.0f+y,  0.0f+z,  0.0f, 0.0f, -1.0f,  col3.x, col3.y, col3.z,  0.0f, 0.0f);
                    ms.vertex( 0.0f+x,  1.0f+y,  0.0f+z,  0.0f, 0.0f, -1.0f,  col1.x, col1.y, col1.z,  0.0f, 1.0f);
                    ms.vertex( 1.0f+x,  1.0f+y,  0.0f+z,  0.0f, 0.0f, -1.0f,  col2.x, col2.y, col2.z,  1.0f, 1.0f);
                }
                if (getBlock(x, y, z + 1) == 0 && getBlock(x, y, z) != 0) { //+z face
                    glm::vec3 col1 = glm::vec3(1.0f); //neg x pos y
                    glm::vec3 col2 = glm::vec3(1.0f); //pos x pos y
                    glm::vec3 col3 = glm::vec3(1.0f); //neg x neg y
                    glm::vec3 col4 = glm::vec3(1.0f); //pos x neg y

                    if (getBlock(x - 1, y, z + 1) != 0 || getBlock(x - 1, y + 1, z + 1) != 0 || getBlock(x, y + 1, z + 1) != 0)
                        col1 = glm::vec3(0.8f);
                    if (getBlock(x + 1, y, z + 1) != 0 || getBlock(x + 1, y + 1, z + 1) != 0 || getBlock(x, y + 1, z + 1) != 0)
                        col2 = glm::vec3(0.8f);
                    if (getBlock(x - 1, y, z + 1) != 0 || getBlock(x - 1, y - 1, z + 1) != 0 || getBlock(x, y - 1, z + 1) != 0)
                        col3 = glm::vec3(0.8f);
                    if (getBlock(x + 1, y, z + 1) != 0 || getBlock(x + 1, y - 1, z + 1) != 0 || getBlock(x, y - 1, z + 1) != 0)
                        col4 = glm::vec3(0.8f);

                    ms.vertex( 0.0f+x,  0.0f+y,  1.0f+z,  0.0f, 0.0f, 1.0f,  col3.x, col3.y, col3.z,  0.0f, 0.0f);
                    ms.vertex( 1.0f+x,  0.0f+y,  1.0f+z,  0.0f, 0.0f, 1.0f,  col4.x, col4.y, col4.z,  1.0f, 0.0f);
                    ms.vertex( 1.0f+x,  1.0f+y,  1.0f+z,  0.0f, 0.0f, 1.0f,  col2.x, col2.y, col2.z,  1.0f, 1.0f);

                    ms.vertex( 1.0f+x,  1.0f+y,  1.0f+z,  0.0f, 0.0f, 1.0f,  col2.x, col2.y, col2.z,  1.0f, 1.0f);
                    ms.vertex( 0.0f+x,  1.0f+y,  1.0f+z,  0.0f, 0.0f, 1.0f,  col1.x, col1.y, col1.z,  0.0f, 1.0f);
                    ms.vertex( 0.0f+x,  0.0f+y,  1.0f+z,  0.0f, 0.0f, 1.0f,  col3.x, col3.y, col3.z,  0.0f, 0.0f);
                }
            }
        }
    }
    this->mesh = ms.toMesh();
}

void Chunk::render(Shader *shader) {
    glm::mat4 model = glm::mat4();
    shader->uniform("model", glm::translate(model, glm::vec3(chunk_x * 64, (chunk_y * 64), chunk_z * 64)));
    this->mesh->render();
}
