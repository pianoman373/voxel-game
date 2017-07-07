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
#include "Console.hpp"

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

static bool p_open = false;

static std::map<std::string, vec3> playerPositions;

Window Client::window;

void Client::init() {
    json j = Util::loadJsonFile("settings.json");
    Settings::load(j);

    Common::init();

    Renderer::init();

    blockShader.load("resources/blockShader.vsh", "resources/blockShader.fsh");
    texture.load("resources/terrain.png");

    //glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    player = new Player(Common::world);
    player->position = vec3(WORLD_SIZE * CHUNK_SIZE / 2.0f, 50.0f, WORLD_SIZE * CHUNK_SIZE / 2.0f);

    //Common::world.rebuild();
}

void Client::run(std::string username, std::string ip) {
    window.create({1400, 800}, "Voxel Game");

    init();



    NetworkManagerClient::connectToServer(username, ip);

    if (!NetworkManagerClient::isLocal) {
       // Common::world.generate(true);
        //Common::world.rebuild();

        for (int x = 0; x < WORLD_SIZE; x++) {
            for (int y = 0; y < WORLD_HEIGHT; y++) {
                for (int z = 0; z < WORLD_SIZE; z++) {
                    sf::Packet p;
                    int id = 4;
                    p << id << x << y << z;
                    NetworkManagerClient::send(p);
                }
            }
        }
    }

    static Console console;

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


        NetworkManagerClient::serverToClientMutex.lock();
        //if (!NetworkManagerClient::serverToClient.empty()) {
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
            if (id == 3) {
                std::string message;
                p >> message;

                console.AddLog("%s", message.c_str());
            }
            if (id == 4) {
                int x, y, z;
                p >> x >> y >> z;

                std::cout << "received chunk from server" << std::endl;

                Chunk *c = new Chunk(x, y, z, &Common::world);//Common::world.getChunk(x, y, z);

                for (int i = 0; i < CHUNK_SIZE; i++) {
                    for (int j = 0; j < CHUNK_SIZE; j++) {
                        for (int k = 0; k < CHUNK_SIZE; k++) {
                            sf::Int8 b;
                            p >> b;

                            c->setBlock(i, j, k, b);
                            c->rebuild = true;
                        }
                    }
                }

                Common::world.addChunk(x, y, z, c);
            }
        }
        NetworkManagerClient::serverToClient.clear();
        NetworkManagerClient::serverToClientMutex.unlock();

        //<---===rendering===--->//
        Common::world.rebuild();
        Common::world.render(camera, blockShader, texture);

        for (auto const &ref: playerPositions) {
            vec3 v = ref.second;

            Renderer::renderDebugAABB(v - vec3(0.3f, 0.9f, 0.3f), v + vec3(0.3f, 0.9f, 0.3f), vec3(1.0f, 0.0f, 0.0f));
        }

        ImGui::SetNextWindowPos(ImVec2(10,10));
        ImGui::SetNextWindowSize(ImVec2(400, 600));
		
        if (!ImGui::Begin("Example: Fixed Overlay", &p_open, ImVec2(0,0), 0.3f, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings))
        {
            ImGui::End();
            return;
        }
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("Delta %.3f", deltaTime);
        std::string s = "Held block: " + BlockRegistry::getBlock(player->heldBlock)->name;
        ImGui::Text(s.c_str());

        console.Draw();

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
