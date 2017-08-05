#pragma once

#include "Mesh.hpp"
#include "Math.hpp"

#include <vector>

class MeshFactory {
private:
    std::vector<vec3> vertices;
    std::vector<vec3> normals;
    std::vector<vec3> colors;
    std::vector<vec2> uvs;
    int count;

public:
    MeshFactory();

    void vertex(float x, float y, float z, float nx, float ny, float nz, float r, float g, float b, float u, float v);

    void vertex(float x, float y, float z, float nx, float ny, float nz, float u, float v);


    void toMesh(Mesh &mesh);

    void clear();
};
