#include "Client.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"
#include "World.hpp"
#include "json.hpp"
#include "Util.hpp"
#include "Settings.hpp"
#include "Common.hpp"
#include "Player.hpp"
#include "NetworkManagerClient.hpp"
#include "Block.hpp"

#define GLEW_STATIC
#include <GL/glew.h>
#include <imgui.h>
#include <thread>
#include <vector>
#include <map>
#include <string>
#include <lua.hpp>
#include <stdio.h>

static Shader blockShader;
static Texture texture;
static Camera camera;
static Player *player;

static std::map<std::string, vec3> playerPositions;

Window Client::window;

void Client::init() {
    json j = Util::loadJsonFile("settings.json");
    Settings::load(j);

    Common::init();

    Renderer::init();

    blockShader.load("resources/blockShader.vsh", "resources/blockShader.fsh");
    texture.load("resources/blocks.png");

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    player = new Player(Common::world);
    player->position = vec3(WORLD_SIZE * CHUNK_SIZE / 2.0f, 50.0f, WORLD_SIZE * CHUNK_SIZE / 2.0f);

    Common::world.rebuild();
}

static bool p_open = false;

void Client::run(std::string ip) {
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
            if (id == 2) {
                float x, y, z;
                std::string userID;

                p >> x >> y >> z >> userID;

                playerPositions[userID] = vec3(x, y, z);
            }
        }
        NetworkManagerClient::serverToClient.clear();

        //<---===rendering===--->//
        Common::world.rebuild();
        Common::world.render(camera, blockShader, texture);

        for (auto const &ref: playerPositions) {
            vec3 v = ref.second;

            Renderer::renderDebugAABB(v - vec3(0.3f, 0.9f, 0.3f), v + vec3(0.3f, 0.9f, 0.3f), vec3(1.0f, 0.0f, 0.0f));
        }

        ImGui::SetNextWindowPos(ImVec2(10,10));
		
        if (!ImGui::Begin("Example: Fixed Overlay", &p_open, ImVec2(0,0), 0.3f, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings))
        {
            ImGui::End();
            return;
        }
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("Delta %.3f", deltaTime);
        std::string s = "Held block: " + BlockRegistry::getBlock(player->heldBlock)->name;
        ImGui::Text(s.c_str());
        ImGui::End();

        Renderer::flush(camera);

        window.update();
    }

    delete player;
    window.terminate();
}

void Client::scrollBlocks(int direction) {
    player->heldBlock += direction;

    if (player->heldBlock >= BlockRegistry::registeredBlocks()) {
        player->heldBlock = 1;
    }
    else if (player->heldBlock < 1) {
        player->heldBlock = BlockRegistry::registeredBlocks() - 1;
    }
    std::cout << player->heldBlock << std::endl;
}
