#include "Mesh.hpp"
#include <glad/glad.h>

Mesh::Mesh() {

}

Mesh::Mesh(std::vector<vec3> positions, std::vector<unsigned int> indices) {
    this->positions = positions;
    this->indices = indices;
}

Mesh::Mesh(std::vector<vec3> positions, std::vector<vec3> normals, std::vector<unsigned int> indices) {
    this->positions = positions;
    this->normals = normals;
    this->indices = indices;
}

Mesh::Mesh(std::vector<vec3> positions, std::vector<vec3> normals, std::vector<vec2> uvs, std::vector<unsigned int> indices) {
    this->positions = positions;
    this->normals = normals;
    this->uvs = uvs;
    this->indices = indices;
}

Mesh::Mesh(std::vector<vec3> positions, std::vector<vec3> normals, std::vector<vec2> uvs, std::vector<vec3> colors, std::vector<unsigned int> indices) {
    this->positions = positions;
    this->normals = normals;
    this->uvs = uvs;
    this->colors = colors;
    this->indices = indices;
}

void Mesh::generate() {
    if (!VBO) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
    }

    std::cout << VBO << std::endl;

    std::vector<float> data;
    for (unsigned int i = 0; i < positions.size(); ++i)
    {
        data.push_back(positions[i].x);
        data.push_back(positions[i].y);
        data.push_back(positions[i].z);
        if (normals.size() > 0)
        {
            data.push_back(normals[i].x);
            data.push_back(normals[i].y);
            data.push_back(normals[i].z);
        }
        if (uvs.size() > 0)
        {
            data.push_back(uvs[i].x);
            data.push_back(uvs[i].y);
        }
        if (colors.size() > 0)
        {
            data.push_back(colors[i].x);
            data.push_back(colors[i].y);
            data.push_back(colors[i].z);
        }
    }

    length = positions.size();

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
    if (indices.size() > 0)
    {
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        length = indices.size();
    }

    int stride = 3 * sizeof(float);
    if (uvs.size() > 0) stride += 2 * sizeof(float);
    if (normals.size() > 0) stride += 3 * sizeof(float);
    if (colors.size() > 0) stride += 3 * sizeof(float);


    long offset = 0;
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
    offset += 3 * sizeof(float);

    if (normals.size() > 0)
    {
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
        offset += 3 * sizeof(float);
    }
    if (uvs.size() > 0)
    {
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
        offset += 2 * sizeof(float);
    }
    if (colors.size() > 0) {
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
        offset += 3 * sizeof(float);
    }
}

void Mesh::render(int mode) {
    glBindVertexArray(VAO);

    if (EBO) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        glDrawElements(mode, length, GL_UNSIGNED_INT, 0);
    }
    else {
        glDrawArrays(mode, 0, length);
    }

    glBindVertexArray(0);
}
