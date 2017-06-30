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
#include "NetworkManagerClient.hpp"


#define GLEW_STATIC
#include <GL/glew.h>
#include <imgui.h>
#include <thread>
#include <vector>

static Shader blockShader;
static Texture texture;
static Camera camera;
static Player *player;

void Client::init() {
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

static bool p_open = false;

void Client::run(std::string ip) {
    Window window;
    window.create({1400, 800}, "Voxel Game");

    NetworkManagerClient::connectToServer(ip);

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

        for (int i = 0; i < NetworkManagerClient::serverToClient.size(); i++) {
            sf::Packet p = NetworkManagerClient::serverToClient[i];

            int id;
            p >> id;

            if (id == 1) {
                int x, y, z, blockID;
                p >> x >> y >> z >> blockID;
                Common::world.setBlock(x, y, z, blockID);
            }
        }
        NetworkManagerClient::serverToClient.clear();

        //<---===rendering===--->//
        Common::world.rebuild();
        Common::world.render(camera, blockShader, texture);

        ImGui::SetNextWindowPos(ImVec2(10,10));
		
        if (!ImGui::Begin("Example: Fixed Overlay", &p_open, ImVec2(0,0), 0.3f, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings))
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
