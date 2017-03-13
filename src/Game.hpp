#pragma once

#include <GL/glew.h>
#include <map>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>

#include "Shader.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"
#include "Camera.hpp"
#include "Chunk.hpp"
#include "World.hpp"
#include "Framebuffer.hpp"

#define WORLD_SIZE 12
#define WORLD_HEIGHT 3

class Game {
private:
    Shader shader;
    Shader GuiShader;
    Shader ShadowShader;
    Mesh GuiMesh;
    Texture *texture;
    Camera camera;
    Chunk *chunks[WORLD_SIZE][WORLD_HEIGHT][WORLD_SIZE];
    World world;
    GLFWwindow *window;
    Framebuffer shadowBuffer;
    Framebuffer shadowBuffer2;

    int test = 10;
    bool AO = true;

public:
    Game(GLFWwindow *window);

    ~Game();

    void init();

    void update();

    void render();
};
