#include "Client.hpp"
#include "Renderer.hpp"
#include "Window.hpp"
#include "Camera.hpp"
#include "World.hpp"
#include "json.hpp"
#include "Util.hpp"
#include "Settings.hpp"
#include "Common.hpp"
#include "Player.hpp"


#define GLEW_STATIC
#include <GL/glew.h>
#include <imgui.h>


static Shader blockShader;
static Texture texture;
static Window window;
static Camera camera;
static Player *player;

void Client::init() {
    window.create(vec2i(1400, 800), "Voxel Game");

    Common::init();

    Renderer::init();

    blockShader.load("resources/blockShader.vsh", "resources/blockShader.fsh");
    texture.load("resources/blocks.png");

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    json j = Util::loadJsonFile("settings.json");
    Settings::load(j);

    player = new Player(Common::world);
    player->position = vec3(WORLD_SIZE * CHUNK_SIZE / 2.0f, 50.0f, WORLD_SIZE * CHUNK_SIZE / 2.0f);

    Common::world.rebuild();
}

void Client::run() {
    init();

    while(window.isOpen())
    {
        static float deltaTime;
        static float lastFrameTime = window.getTime();

        float currentFrameTime = window.getTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        window.begin();
        window.pollEvents();

        player->update(camera, deltaTime);

        //<---===rendering===--->//
        Common::world.rebuild();
        Common::world.render(camera, blockShader, texture);

        ImGui::SetNextWindowPos(ImVec2(10,10));
        bool *p_open;
        if (!ImGui::Begin("Example: Fixed Overlay", p_open, ImVec2(0,0), 0.3f, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings))
        {
            ImGui::End();
            return;
        }
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        Renderer::flush(camera);

        window.update();
    }

    delete player;
    window.terminate();
}
