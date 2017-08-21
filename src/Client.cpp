#include "Client.hpp"
#include "Settings.hpp"
#include "Util.hpp"
#include "Player.hpp"
#include "Common.hpp"
#include "NetworkManagerClient.hpp"
#include "Console.hpp"
#include "Block.hpp"
#include "World.hpp"
#include "Chunk.hpp"

#include <crucible/Shader.hpp>
#include <crucible/Texture.hpp>
#include <crucible/Renderer.hpp>
#include <crucible/Window.hpp>
#include <crucible/Frustum.hpp>
#include <glad/glad.h>
#include <crucible/Camera.hpp>
#include <SFML/Network.hpp>
#include <crucible/Input.hpp>

static Shader blockShader;
static Shader blockShaderFar;
static Shader skyboxShader;
static Texture texture;
static Camera camera;
static Player *player;
static Console console;

Frustum Client::frustum;

static Material nearMaterial;
static Material farMaterial;

static bool p_open = false;

static std::map<int, vec3> playerPositions;

void Client::renderGUI(float deltaTime) {
    ImGui::SetNextWindowPos(ImVec2(10,10));
    ImGui::SetNextWindowSize(ImVec2(400, 600));

    if (!ImGui::Begin("Example: Fixed Overlay", &p_open, ImVec2(0,0), 0.3f, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings))
    {
        ImGui::End();
        return;
    }
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text("Delta %.3f", deltaTime);
    ImGui::Text("PlayerPos: %.2f %.2f %.2f", player->position.x, player->position.y, player->position.z);
    std::string s = "Held block: " + BlockRegistry::getBlock(player->heldBlock)->name;
    ImGui::Text("%s", s.c_str());

    console.Draw();

    ImGui::End();
}

void Client::handlePackets() {
    NetworkManagerClient::serverToClientMutex.lock();
    for (unsigned int i = 0; i < NetworkManagerClient::serverToClient.size(); i++) {
        sf::Packet p = NetworkManagerClient::serverToClient[i];

        int id;
        p >> id;

        if (id == -1) {
            int userID;
            p >> userID;

            std::cout << "got message from server that user #" << userID << " disconnected" << std::endl;
            playerPositions.erase(userID);
        }
        if (id == 1) {
            int x, y, z, blockID;
            p >> x >> y >> z >> blockID;
            Common::world.setBlock(x, y, z, blockID);
        }
        if (id == 2) {
            float x, y, z;
            int userID;

            p >> x >> y >> z >> userID;

            playerPositions[userID] = vec3(x, y, z);
        }
        if (id == 3) {
            std::string message;
            p >> message;

            console.AddLog("%s", message.c_str());
        }
        if (id == 4) {
            std::cout << "received chunk from server" << std::endl;

            bool isEmpty;
            int x, y, z;

            p >> isEmpty >> x >> y >> z;

             Chunk *c = new Chunk(x, y, z, &Common::world);

            if (!isEmpty) {


                for (int i = 0; i < CHUNK_SIZE; i++) {
                    for (int j = 0; j < CHUNK_SIZE; j++) {
                        for (int k = 0; k < CHUNK_SIZE; k++) {
                            sf::Int8 b;
                            p >> b;

                            c->setBlock(i, j, k, b);
                        }
                    }
                }
                c->rebuild = true;
            }

            Common::world.addChunk(x, y, z, c);
        }
    }
    NetworkManagerClient::serverToClient.clear();
    NetworkManagerClient::serverToClientMutex.unlock();
}

void Client::init() {
    json j = Util::loadJsonFile("settings.json");
    Settings::load(j);

    Common::init();
    Renderer::init(Settings::shadows, Settings::shadow_resolution);
    Renderer::setSun({normalize(vec3(-0.4f, -0.7f, -1.0f)), vec3(1.4f, 1.3f, 1.0f)});

    blockShader.loadFile("resources/blockShader.vsh", "resources/blockShader.fsh");
    blockShaderFar.loadFile("resources/blockShader.vsh", "resources/blockShaderSimple.fsh");
    skyboxShader.loadFile("resources/skybox.vsh", "resources/skybox.fsh");
    texture.load("resources/terrain.png");

    nearMaterial.setShader(blockShader);
    nearMaterial.setUniformTexture("tex4", texture, 4);

    farMaterial.setShader(blockShaderFar);

    Renderer::setSkyboxShader(skyboxShader);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    player = new Player(Common::world);
    player->position = vec3(16.0f, 130.0f, 16.0f);
}

void Client::run(std::string username, std::string ip) {
    Window::create({1400, 800}, "Voxel Game");

    init();


    NetworkManagerClient::connectToServer(username, ip);

    while (Window::isOpen()) {
        static float deltaTime;
        static float lastFrameTime = Window::getTime();

        float currentFrameTime = Window::getTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        camera.dimensions = {Window::getWindowSize().x, Window::getWindowSize().y};

        Window::begin();
        scrollBlocks(Input::getScroll());

        player->update(camera, deltaTime);
        Common::world.update(camera, deltaTime);

        //receive packets
        handlePackets();

        vec2i size = Window::getWindowSize();

        if (Input::isKeyDown(GLFW_KEY_F) || true) {
            frustum.setupInternals(Settings::fov, (float) size.x / (float) size.y, 0.1f, 1000.0f);
            float radius = 20.0f;
            //frustum.setupInternalsOrthographic(-radius, radius, -radius, radius, -radius * 8, radius * 8);
            frustum.updateCamPosition(camera);
        }

        //<---===rendering===--->//
        Common::world.rebuild();
        Common::world.render(camera, &nearMaterial, &farMaterial);
        //frustum.renderDebug();

        for (auto const &ref: playerPositions) {
            vec3 v = ref.second;

            Renderer::renderDebugAABB(v - vec3(0.3f, 0.9f, 0.3f), v + vec3(0.3f, 0.9f, 0.3f), vec3(1.0f, 0.0f, 0.0f));
        }

        renderGUI(deltaTime);

        //render scene and update window
        Renderer::flush(camera);
        Window::end();
    }

    shutdown();
}

void Client::scrollBlocks(int direction) {
    player->heldBlock += direction;

    if (player->heldBlock >= BlockRegistry::registeredBlocks()) {
        player->heldBlock = 1;
    }
    else if (player->heldBlock < 1) {
        player->heldBlock = BlockRegistry::registeredBlocks() - 1;
    }
}

void Client::shutdown() {
    NetworkManagerClient::disconnect();
    Window::terminate();
    exit(0);
}
