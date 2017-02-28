#include "Mesh.hpp"

#include <vector>

class MeshFactory {
private:
    std::vector<float> vertices;
    int count;

public:
    MeshFactory();

    void vertex(float x, float y, float z, float nx, float ny, float nz, float r, float g, float b, float u, float v);

    void vertex(float x, float y, float z, float nx, float ny, float nz, float u, float v);


    Mesh* toMesh();
};
