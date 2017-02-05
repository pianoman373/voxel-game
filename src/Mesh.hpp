#pragma once

class Mesh {
private:
    unsigned int VAO;
    unsigned int VBO;
    int size;

public:
    Mesh(float *vertices, int size);

    ~Mesh();

    void render();
};
