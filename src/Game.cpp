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

Game::Game() {
}

Game::~Game() {
    delete shader;
    delete texture;
    delete camera;
    delete[] chunks;
}

void placeBlocks(Chunk *chunk) {
    for (int x = 0; x < 64; x++) {
        for (int z = 0; z < 64; z++) {
            float height = glm::simplex(glm::vec2((x + (chunk->chunk_x * 64)) / 100.0, (z + (chunk->chunk_z * 64)) / 100.0)) * 10 + 30;
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
    std::cout << camera->getPosition().x << std::endl;
    shader->uniform("cameraPos", camera->getPosition());

    for (int x = 0; x < 8; x++) {
        for (int z = 0; z < 8; z++) {
            chunks[x][z]->render(shader);
        }
    }
}
