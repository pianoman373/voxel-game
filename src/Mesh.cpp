#include "Mesh.hpp"

#include <GL/glew.h>
#include <iostream>

void Mesh::initialize(GLfloat *vertices, int size, MeshMode mode, int meshStream) {
    this->size = size;
    //setup the model

    if (VAO == 0) {
        glGenVertexArrays(1, &VAO);
    }

    glBindVertexArray(VAO);
        if (mode == MeshMode::BLOCKS) {
            if (VBO == 0) {
                glGenBuffers(1, &VBO);
            }
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, this->size * 12 * sizeof(float), vertices, meshStream);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (GLvoid*)0);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (GLvoid*)(3* sizeof(float)));
            glEnableVertexAttribArray(1);

            glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (GLvoid*)(6* sizeof(float)));
            glEnableVertexAttribArray(2);

            glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (GLvoid*)(10* sizeof(float)));
            glEnableVertexAttribArray(3);
        }
        if (mode == MeshMode::SIMPLE_TEXTURED) {
            if (VBO == 0) {
                glGenBuffers(1, &VBO);
            }
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, this->size * 5 * sizeof(float), vertices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)0);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)(3* sizeof(float)));
            glEnableVertexAttribArray(1);
        }
    glBindVertexArray(0);
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

Mesh::Mesh() {

}

void Mesh::render(int mode) {
    glBindVertexArray(VAO);
    glDrawArrays(mode, 0, this->size);
    glBindVertexArray(0);
}
