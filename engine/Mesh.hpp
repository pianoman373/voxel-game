#pragma once

enum MeshMode {
    BLOCKS,
    SIMPLE_TEXTURED
};

class Mesh {
private:
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    int size;

public:
   ~Mesh();

    Mesh();

    //                                                          GL_STATIC_DRAW |
    //                                                                         V
    void initialize(float *vertices, int size, MeshMode mode, int meshStream=0x88E4);

    void render(int mode=0x0004);
};
