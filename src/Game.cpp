#include "Game.hpp"
#include "Input.hpp"
#include "MeshFactory.hpp"

#include <iostream>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>

#include <vector>
#include <array>
#include <cmath>


Game::Game() {
}

Game::~Game() {
    delete shader;
    delete texture;
    delete camera;
    delete[] chunks;
}

float noise(glm::vec3 position, int octaves, float frequency, float persistence) {
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

float ridgedNoise(glm::vec3 position, int octaves, float frequency, float persistence) {
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
    for (int x = 0; x < 64; x++) {
        for (int z = 0; z < 64; z++) {
            //float height = glm::simplex(glm::vec2((x + (chunk->chunk_x * 64)) / 100.0, (z + (chunk->chunk_z * 64)) / 100.0)) * 40 + 20;
            float height = ridgedNoise(glm::vec3(x + (chunk->chunk_x * 64), 0, z + (chunk->chunk_z * 64)), 11, 0.006f, 0.5f) * 50.0f + 10.0f;
            //std::cout << height << std::endl;
            int iHeight = static_cast<int>(height);
            for (int y = 0; y < 64; y++) {
                if (y < iHeight) {
                    chunk->setBlock(x, y, z, 1);
                }
                else {
                    chunk->setBlock(x, y, z, 0);
                }
            }
        }
    }

    chunk->generateMesh();
}

void Game::init() {
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    for (int x = 0; x < 8; x++) {
        for (int z = 0; z < 8; z++) {
            chunks[x][z] = new Chunk(x - 4, 0, z - 4);

            placeBlocks(chunks[x][z]);
        }
    }

    shader = new Shader("resources/shader.vsh", "resources/shader.fsh");
    texture = new Texture("resources/crate.png");
    camera = new Camera();
}

void Game::update() {
    camera->update();
}

void Game::render() {

    glm::mat4 view = camera->getView();
    glm::mat4 projection = camera->getProjection();

    texture->bind();
    shader->bind();
    shader->uniform("view", view);
    shader->uniform("projection", projection);
    shader->uniform("cameraPos", camera->getPosition());

    for (int x = 0; x < 8; x++) {
        for (int z = 0; z < 8; z++) {
            chunks[x][z]->render(shader);
        }
    }
}
