#define GLEW_STATIC
#include <GL/glew.h>

#include <iostream>
#include <fstream>
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
#include "Block.hpp"
#include "Settings.hpp"
#include "json.hpp"

#define WORLD_SIZE 16
#define WORLD_HEIGHT 2

Window window;
Shader blockShader;
Shader GuiShader;
World world;
Texture texture;
Camera camera;

static float heightAt(vec3 pos) {
    return Util::ridgedNoise(vec2(pos.x, pos.z), 5, 0.003f, 0.5f) * 30.0f + 20.0f;
}

static void placeBlocks(Chunk *chunk) {
    //std::cout << "generating chunk" << std::endl;

    //double time1 = glfwGetTime();
    for (int x = 0; x < CHUNK_SIZE; x++) {
        //for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                float height = heightAt(vec3(x + (chunk->chunk_x * CHUNK_SIZE), 0.0f, z + (chunk->chunk_z * CHUNK_SIZE)));

                //create vectors which begin at this block and extend to the next block over
                vec3 lineNegX =  normalize(vec3(x - 1, heightAt(vec3(x + (chunk->chunk_x * CHUNK_SIZE) - 1, 0.0f, z + (chunk->chunk_z * CHUNK_SIZE))), z) - vec3(x, height, z));
                vec3 linePosX =  normalize(vec3(x + 1, heightAt(vec3(x + (chunk->chunk_x * CHUNK_SIZE) + 1, 0.0f, z + (chunk->chunk_z * CHUNK_SIZE))), z) - vec3(x, height, z));

                vec3 lineNegY =  normalize(vec3(x, heightAt(vec3(x + (chunk->chunk_x * CHUNK_SIZE), 0.0f, z + (chunk->chunk_z * CHUNK_SIZE) - 1)), z - 1) - vec3(x, height, z));
                vec3 linePosY =  normalize(vec3(x, heightAt(vec3(x + (chunk->chunk_x * CHUNK_SIZE), 0.0f, z + (chunk->chunk_z * CHUNK_SIZE) + 1)), z + 1) - vec3(x, height, z));

                //cross those vectors to get upwards pointing lines
                vec3 norm1 = cross(lineNegY, lineNegX);
                vec3 norm2 = cross(linePosX, lineNegY);

                vec3 norm3 = cross(linePosY, linePosX);
                vec3 norm4 = cross(lineNegX, linePosY);


                //average them together to get what is effectively a surface normal if this terrain were to be smoothed out
                vec3 finalNorm = (norm1 + norm2 + norm3 + norm4) / 4.0f;


                //std::cout << lineNegX << std::endl;

                height -= (chunk->chunk_y * 32);
                for (int i = 0; i < height; i++) {
                    if (dot(finalNorm, vec3(0.0f, 1.0f, 0.0f)) < 0.7f) {
                        chunk->setBlock(x, i, z, 1);
                    }
                    else {
                        chunk->setBlock(x, i, z, 2);
                    }
                }
                if (dot(finalNorm, vec3(0.0f, 1.0f, 0.0f)) < 0.7f) {
                    chunk->setBlock(x, floor(height), z, 1);
                }
                else {
                    chunk->setBlock(x, floor(height), z, 3);
                }
            }
        //}
    }
    //std::cout << "generated chunk in: " << glfwGetTime() - time1 << std::endl;
    chunk->rebuild = true;
    //chunk->generateMesh();
}

static void init() {
    json j = Util::loadJsonFile("settings.json");
    Settings::load(j);

    Renderer::init();

    BlockRegistry::registerBlock(0, new BlockAir());
    BlockRegistry::registerBlock(1, new SimpleBlock(vec2i(0, 0)));
    BlockRegistry::registerBlock(2, new SimpleBlock(vec2i(4, 0)));
    BlockRegistry::registerBlock(3, new BlockGrass());
    BlockRegistry::registerBlock(4, new SimpleBlock(vec2i(2, 0)));

    blockShader.load("resources/blockShader.vsh", "resources/blockShader.fsh");
    texture.load("resources/blocks.png");

    //glEnable(GL_CULL_FACE);
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
    world.update(camera, deltaTime);

    //<---===rendering===--->//
    world.rebuild();

    world.render(camera, blockShader, texture);

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
