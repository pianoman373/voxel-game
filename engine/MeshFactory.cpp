#include "MeshFactory.hpp"
#include "iostream"
#include <glad/glad.h>

MeshFactory::MeshFactory() {
    count = 0;
}

void MeshFactory::vertex(float x, float y, float z, float nx, float ny, float nz, float r, float g, float b, float u, float v) {
    vertices.push_back({x, y, z});

    normals.push_back({nx, ny, nz});

    colors.push_back({r, g, b});

    uvs.push_back({u, v});
    count++;
}

void MeshFactory::vertex(float x, float y, float z, float nx, float ny, float nz, float u, float v) {
    vertex(x, y, z, nx, ny, nz, 0.0f, 0.0f, 0.0f, u, v);
}

void MeshFactory::toMesh(Mesh &mesh) {
    mesh.positions = vertices;
    mesh.normals = normals;
    mesh.colors = colors;
    mesh.uvs = uvs;
    mesh.generate();
}

void MeshFactory::clear() {
    vertices.clear();
    normals.clear();
    colors.clear();
    uvs.clear();
    count = 0;
}
