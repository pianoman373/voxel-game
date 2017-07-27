#include "MeshFactory.hpp"
#include "iostream"
#include <glad/glad.h>

MeshFactory::MeshFactory() {
    vertices = std::vector<float>();
    count = 0;
}

void MeshFactory::vertex(float x, float y, float z, float nx, float ny, float nz, float r, float g, float b, float a, float u, float v) {
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z);

    vertices.push_back(nx);
    vertices.push_back(ny);
    vertices.push_back(nz);

    vertices.push_back(r);
    vertices.push_back(g);
    vertices.push_back(b);
    vertices.push_back(a);

    vertices.push_back(u);
    vertices.push_back(v);
    count++;
}

void MeshFactory::vertex(float x, float y, float z, float nx, float ny, float nz, float u, float v) {
    vertex(x, y, z, nx, ny, nz, 0.0f, 0.0f, 0.0f, 0.0f, u, v);
}

void MeshFactory::toMesh(Mesh &mesh) {
    mesh.initialize(vertices.data(), count, MeshMode::BLOCKS, GL_DYNAMIC_DRAW);
}

void MeshFactory::clear() {
 vertices.clear();
 count = 0;
}