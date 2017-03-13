#pragma once

enum MeshMode {
    BLOCKS,
    SIMPLE_TEXTURED
};

class Mesh {
private:
    unsigned int VAO;
    unsigned int VBO;
    int size;

public:
   ~Mesh();

    void initialize(float *vertices, int size, MeshMode mode);

    void render();
};
