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
#include <thread>
#include <vector>


sf::UdpSocket Client::socket;
sf::IpAddress Client::connectedServer;

struct Message {
    sf::Packet packet;
    sf::IpAddress sender;
    unsigned short port;
};

std::vector<Message> messageStack;


static Shader blockShader;
static Texture texture;
static Camera camera;
static Player *player;

void Client::handleIncomingPackets() {
    while (true) {
        sf::Packet packet;

        sf::IpAddress sender;
        unsigned short port;

        if (socket.receive(packet, sender, port) != sf::Socket::Done)
        {
            // error...
        }

        std::cout << "Received message from " << sender << " on port " << port << std::endl;

        Message message = {packet, sender, port};
        messageStack.push_back(message);
    }
}

void Client::init() {
    // bind the socket to a port
    if (socket.bind(socket.AnyPort) != sf::Socket::Done)
    {
        // error...
    }

    //send connection packet
    //we don't need a confirmation packet (yet)
    sf::Packet packet;
    int id = 0;
    packet << id;
    socket.send(packet, connectedServer, 54000);

    std::thread thread(handleIncomingPackets);
    thread.detach();

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

    connectedServer = ip;
    init();



    while(window.isOpen())
    {
        static float deltaTime;
        static float lastFrameTime = window.getTime();

        float currentFrameTime = window.getTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;
        deltaTime = 0.01f;

        window.begin();
        window.pollEvents();

        player->update(camera, deltaTime);

        for (int i = 0; i < messageStack.size(); i++) {
            sf::Packet p = messageStack[i].packet;

            int x, y, z, blockID;
            p >> x >> y >> z >> blockID;
            Common::world.setBlock(x, y, z, blockID);
        }
        messageStack.clear();

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
