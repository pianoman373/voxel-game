#include "Game.hpp"
#include "Input.hpp"
#include "MeshFactory.hpp"

#include <iostream>

#include <vector>
#include <array>
#include <cmath>
#include <imgui.h>

float vertices[] = {
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f,

    0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 0.0f, 1.0f
};


Game::Game(GLFWwindow *window) {
    this->window = window;
}

Game::~Game() {
    delete texture;

    for (int x = 0; x < WORLD_SIZE; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            for (int z = 0; z < WORLD_SIZE; z++) {
                delete chunks[x][y][z];
            }
        }
    }
}

float noise(glm::vec2 position, int octaves, float frequency, float persistence) {
    float total = 0.0;
    float maxAmplitude = 0.0;
    float amplitude = 1.0;
    for (int i = 0; i < octaves; i++) {
        total += glm::simplex(position * frequency) * amplitude;
        frequency *= 2.0;
        maxAmplitude += amplitude;
        amplitude *= persistence;
    }
    return total / maxAmplitude;
}

float ridgedNoise(glm::vec2 position, int octaves, float frequency, float persistence) {
    float total = 0.0f;
    float maxAmplitude = 0.0f;
    float amplitude = 1.0f;
    for (int i = 0; i < octaves; i++) {
        total += ((1.0f - fabs(glm::simplex(position * frequency))) * 2.0f - 1.0f) * amplitude;
        frequency *= 2.0f;
        maxAmplitude += amplitude;
        amplitude *= persistence;
    }

    return total / maxAmplitude;
}

void placeBlocks(Chunk *chunk) {
    std::cout << "generating chunk" << std::endl;

    double time1 = glfwGetTime();
    for (int x = 0; x < 32; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            for (int z = 0; z < 32; z++) {
                float height = ridgedNoise(glm::vec2(x + (chunk->chunk_x * 32), z + (chunk->chunk_z * 32)), 5, 0.003f, 0.5f) * 60.0f + 30.0f;
                height -= (y + chunk->chunk_y * 32);
                for (int y = 0; y < height; y++) {
                    chunk->setBlock(x, y, z, 1);
                }
            }
        }
    }
    std::cout << "generated chunk in: " << glfwGetTime() - time1 << std::endl;

    double time2 = glfwGetTime();
    chunk->generateMesh();
    std::cout << "generated mesh in: " << glfwGetTime() - time2 << std::endl;
}


void Game::init() {
    shader.load("resources/shader.vsh", "resources/shader.fsh");
    GuiShader.load("resources/gui.vsh", "resources/gui.fsh");
    ShadowShader.load("resources/shadow.vsh", "resources/shadow.fsh");
    GuiMesh.initialize(vertices, 6, MeshMode::SIMPLE_TEXTURED);
    texture = new Texture("resources/clouds.png");
    shadowBuffer.setupShadow(4096, 4096);
    shadowBuffer2.setupShadow(4096, 4096);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    for (int x = 0; x < WORLD_SIZE; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            for (int z = 0; z < WORLD_SIZE; z++) {
                chunks[x][y][z] = new Chunk(x - 1, y, z - 1);

                placeBlocks(chunks[x][y][z]);
            }
        }
    }
}

void Game::update() {
    static bool cursorMode = false;
    static bool spaceDown = false;

    camera.update();
    if (cursorMode)
        camera.updateMouse();

    if (Input::isKeyDown(GLFW_KEY_SPACE)) {
        if (!spaceDown) {
            cursorMode = !cursorMode;
            glfwSetInputMode(window, GLFW_CURSOR, cursorMode ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        }
        spaceDown = true;
    }
    else {
        spaceDown = false;
    }
}

void Game::render() {
    //shadow buffer
    glViewport(0, 0, 4096, 4096);
    shadowBuffer.bind();
    glClear(GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = camera.getView();
    glm::mat4 projection = camera.getProjection();
    glm::mat4 lightProjection = glm::ortho(-200.0f, 200.0f, -200.0f, 200.0f, -200.0f, 200.0f);
    glm::mat4 lightView = glm::lookAt(camera.getPosition() + glm::vec3(0.9, 1.1, 1.0),
                                      camera.getPosition(),
                                      glm::vec3( 0.0f, 1.0f,  0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    ShadowShader.bind();
    ShadowShader.uniform("lightSpaceMatrix", lightSpaceMatrix);

    for (int x = 0; x < WORLD_SIZE; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            for (int z = 0; z < WORLD_SIZE; z++) {
                chunks[x][y][z]->render(ShadowShader);
            }
        }
    }

    //normal rendering
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, 1500, 900);

    shadowBuffer.bindTexture();

    shader.bind();
    shader.uniform("view", view);
    shader.uniform("projection", projection);
    shader.uniform("cameraPos", camera.getPosition());
    shader.uniform("lightSpaceMatrix", lightSpaceMatrix);

    for (int x = 0; x < WORLD_SIZE; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            for (int z = 0; z < WORLD_SIZE; z++) {
                chunks[x][y][z]->render(shader);
            }
        }
    }

    static float f = 0.0f;
    ImGui::Text("Hello, world!");
    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
    ImGui::Image(ImTextureID(shadowBuffer.getTextureID()), ImVec2(256, 256), ImVec2(0,1), ImVec2(1,0), ImColor(255,255,255,255), ImColor(255,255,255,128));
    //ImGui::Image

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}
