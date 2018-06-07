#include "ChunkMesh.hpp"

#include <glad/glad.h>

void ChunkMesh::pushVertex(float positionX, float positionY, float positionZ, float u, float v, float ao, unsigned int index) {
    buffer.push_back(positionX);
    buffer.push_back(positionY);
    buffer.push_back(positionZ);

    buffer.push_back(u);
    buffer.push_back(v);

    buffer.push_back(ao);

    // Since integers and floats both take up 4 bytes in opengl we can actually store an integer value in a float buffer.
    // This is crazy but it seems to work great.
    float f;
    *(unsigned int *)&f = index;

    buffer.push_back(f);

    bufferLength++;
}

void ChunkMesh::generate() {
    if (!VBO) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
    }

    data = buffer;
    length = bufferLength;
    bufferLength = 0;
    buffer.clear();

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);


    int stride = 7 * sizeof(float);


    long offset = 0;
    //positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
    offset += 3 * sizeof(float);

    //uvs
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
    offset += 2 * sizeof(float);

    //ao
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
    offset += 1 * sizeof(unsigned int);

    //normal index
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, stride, (GLvoid*)offset);
    offset += 1 * sizeof(unsigned int);
}

void ChunkMesh::clear() {
    data.clear();
    buffer.clear();
    length = 0;
    bufferLength = 0;
}

void ChunkMesh::render() {
    glBindVertexArray(VAO);


    glDrawArrays(GL_TRIANGLES, 0, length);

    glBindVertexArray(0);
}

void ChunkMesh::destroy() {
    clear();
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}