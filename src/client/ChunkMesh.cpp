#include "client/ChunkMesh.hpp"

#include <glad/glad.h>

void ChunkMesh::pushVertex(uint16_t positionX, uint16_t positionY, uint16_t positionZ, float u, float v, float w, uint8_t ao, uint8_t index) {
    float f;
    uint32_t final = 0;
    final |= (positionX);
    final |= (positionY << 16);

    *(uint32_t*)&f = final;
    buffer.push_back(f);


    final = 0;
    final |= (positionZ);
    final |= (ao << 16);
    final |= ( index << 24 );

    *(uint32_t*)&f = final;
    buffer.push_back(f);


    buffer.push_back(u);
    buffer.push_back(v);
    buffer.push_back(w);

    bufferLength++;
}

void ChunkMesh::pushFace(uint16_t positionX1, uint16_t positionY1, uint16_t positionZ1, float u1, float v1, uint8_t ao1, uint8_t index1,
                         uint16_t positionX2, uint16_t positionY2, uint16_t positionZ2, float u2, float v2, uint8_t ao2, uint8_t index2,
                         uint16_t positionX3, uint16_t positionY3, uint16_t positionZ3, float u3, float v3, uint8_t ao3, uint8_t index3,
                         uint16_t positionX4, uint16_t positionY4, uint16_t positionZ4, float u4, float v4, uint8_t ao4, uint8_t index4,
                         float layer) {

    indicesBuffer.push_back(bufferLength);
    pushVertex(positionX1, positionY1, positionZ1, u1, v1, layer, ao1, index1);

    indicesBuffer.push_back(bufferLength);
    pushVertex(positionX2, positionY2, positionZ2, u2, v2, layer, ao2, index2);

    indicesBuffer.push_back(bufferLength);
    pushVertex(positionX3, positionY3, positionZ3, u3, v3, layer, ao3, index3);


    indicesBuffer.push_back(bufferLength-1);

    indicesBuffer.push_back(bufferLength);
    pushVertex(positionX4, positionY4, positionZ4, u4, v4, layer, ao4, index4);

    indicesBuffer.push_back(bufferLength-4);
}

void ChunkMesh::generate() {

    if (!VBO) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
    }

    data = buffer;
    indices = indicesBuffer;
    length = bufferLength;
    bufferLength = 0;
    buffer.clear();
    buffer.shrink_to_fit();
    indicesBuffer.clear();
    indicesBuffer.shrink_to_fit();
    data.shrink_to_fit();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);


    int stride = 20;


    long offset = 0;
    //positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_UNSIGNED_SHORT, GL_FALSE, stride, (GLvoid*)offset);
    offset += 6;

    //ao
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_UNSIGNED_BYTE, GL_TRUE, stride, (GLvoid*)offset);
    offset += 1;

    //normal index
    glEnableVertexAttribArray(2);
    glVertexAttribIPointer(2, 1, GL_UNSIGNED_BYTE, stride, (GLvoid*)offset);
    offset += 1;


    //uvs
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_TRUE, stride, (GLvoid*)offset);
}

void ChunkMesh::clear() {
    data.clear();
    buffer.clear();

    indices.clear();
    indicesBuffer.clear();

    length = 0;
    bufferLength = 0;
}

void ChunkMesh::render() const {
    glBindVertexArray(VAO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void ChunkMesh::destroy() {
    clear();
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}