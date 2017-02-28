#pragma once

#include <GL/glew.h>
#include "Shader.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"
#include "Camera.hpp"
#include "Chunk.hpp"

class Game {
private:
    Shader *shader;
    Mesh *mesh;
    Texture *texture;
    Camera *camera;
    Chunk *chunks[8][8];

public:
    Game();

    ~Game();

    void init();

    void update();

    void render();
};
