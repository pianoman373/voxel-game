#define GLEW_STATIC
#include <GL/glew.h>

#include <iostream>
#include <imgui.h>

#ifdef __linux__
#include <unistd.h>
#endif

#include "Window.hpp"
#include "Shader.hpp"
#include "Chunk.hpp"`
#include "World.hpp"
#include "Util.hpp"
#include "Framebuffer.hpp"
#include "Texture.hpp"
#include "Camera.hpp"
#include "Material.hpp"
#include "Renderer.hpp"
#include "AABB.hpp"

#define WORLD_SIZE 8
#define WORLD_HEIGHT 2

Window window;
Shader shader;
Shader GuiShader;
World world;
Texture texture;
Camera camera;

static void placeBlocks(Chunk *chunk) {
    //std::cout << "generating chunk" << std::endl;

    //double time1 = glfwGetTime();
    for (int x = 0; x < 32; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            for (int z = 0; z < 32; z++) {
                float height = Util::ridgedNoise(vec2(x + (chunk->chunk_x * 32), z + (chunk->chunk_z * 32)), 5, 0.003f, 0.5f) * 20.0f + 20.0f;
                height -= (y + chunk->chunk_y * 32);
                for (int y = 0; y < height; y++) {
                    chunk->setBlock(x, y, z, 1);
                }
            }
        }
    }
    //std::cout << "generated chunk in: " << glfwGetTime() - time1 << std::endl;
    chunk->rebuild = true;
    //chunk->generateMesh();
}

static void init() {
    Renderer::init();

    shader.load("resources/shader.vsh", "resources/shader.fsh");
    texture.load("resources/blocks.png");

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    for (int x = 0; x < WORLD_SIZE; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            for (int z = 0; z < WORLD_SIZE; z++) {
                Chunk *c = new Chunk(x, y, z, &world);
                placeBlocks(c);
                world.addChunk(x, y, z, c);
            }
        }
    }

    world.rebuild();

    world.player.position = vec3(WORLD_SIZE * CHUNK_SIZE / 2.0f, 50.0f, WORLD_SIZE * CHUNK_SIZE / 2.0f);
}

static void update(float deltaTime) {


    camera.update(window.window, deltaTime);

    world.update(camera, deltaTime);

    //<---===rendering===--->//
    world.rebuild();

    world.render(shader, texture);

    ImGui::SetNextWindowPos(ImVec2(10,10));
    bool *p_open;
    if (!ImGui::Begin("Example: Fixed Overlay", p_open, ImVec2(0,0), 0.3f, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings))
    {
        ImGui::End();
        return;
    }
    //ImGui::Text("Mouse Position: (%.1f,%.1f)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();


}

int main() {
    window.create(vec2i(1400, 800), "Voxel Game");

    init();

    while(window.isOpen())
	{   
        static float deltaTime;
        static float lastFrameTime = window.getTime();

        float currentFrameTime = window.getTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        //usleep(40000);
        window.begin();
        window.pollEvents();


        update(deltaTime);



        Renderer::flush(camera);


        window.update();
	}

    window.terminate();
    return 1;
}
