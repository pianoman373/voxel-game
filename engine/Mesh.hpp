#pragma once

#include <vector>
#include <string>

#include "Math.hpp"


class Mesh {
private:
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;

    int length = 0;

public:
    std::vector<vec3> positions;
    std::vector<vec2> uvs;
    std::vector<vec3> normals;
    std::vector<vec3> colors;
    std::vector<unsigned int> indices;

    Mesh();

    Mesh(std::vector<vec3> positions, std::vector<unsigned int> indices);

    Mesh(std::vector<vec3> positions, std::vector<vec3> normals, std::vector<unsigned int> indices);

    Mesh(std::vector<vec3> positions, std::vector<vec3> normals, std::vector<vec2> uvs, std::vector<unsigned int> indices);

    Mesh(std::vector<vec3> positions, std::vector<vec3> normals, std::vector<vec2> uvs, std::vector<vec3> colors, std::vector<unsigned int> indices);

    void generate();

    void render(int mode=0x0004);
};
