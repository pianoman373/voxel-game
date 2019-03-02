#include "Client.hpp"
#include "NetworkManagerClient.hpp"
#include "Noise.hpp"

#include <crucible/Window.hpp>
#include <crucible/Renderer.hpp>
#include <crucible/Primitives.hpp>
#include <crucible/Util.hpp>
#include <crucible/Input.hpp>

#include <cstring>

#include <enet/enet.h>

#include "Packet.hpp"
#include "Block.hpp"

#include <typeinfo>
#include <typeindex>

#include <strstream>



Client::Client(): network(*this), worldRenderer(world), player(world, *this) {
    rleCache = new uint8_t[16*16*256*2];
}

Client::~Client() {
    delete[] rleCache;
}

void Client::requestChunkFromServer(int x, int z) {
    Packet p;
    uint16_t packetID = 3;

    p << packetID << x << z;

    network.sendPacket(p);

}

void Client::receivePackets() {
    network.processEvents();

    static long bytesReceived = 0;

    for (int i = 0; i < network.getNumPackets(); i++) {
        Packet &p = network.getPacket(i);

        uint16_t packetID;
        p >> packetID;

        if (packetID == 0) {

        }
        if (packetID == 1) {
            std::string s;
            p >> s;

            std::cout << "(Client) Message from server : " << s << std::endl;
        }
        if (packetID == 2) { //player position data
            uint16_t playerID;
            float x, y, z;

            p >> playerID >> x >> y >> z;

            //std::cout << "received position data for player (" << playerID << ") " << x << ", " << y << ", " << z << std::endl;

            playerPositions[playerID] = vec3(x, y, z);
        }
        if (packetID == 3) { //chunk data
            int x, z;
            uint16_t length;

            p >> x >> z >> length;

            //std::cout << "(Client) Received chunk at: " << x << ", " << z << std::endl;
            bytesReceived += length;
            //std::cout << "Received a total of " << bytesReceived << " bytes." << std::endl;

            std::shared_ptr<Chunk> chunk = world.getChunk(x, z);

            p.releaseData(rleCache, length);

            chunk->unSerialize(rleCache, length);

            chunk->isDirty = true;

            worldRenderer.getChunkRenderer(x, z);
        }
        if (packetID == 4) {
            int x, y, z, blockID;

            p >> x >> y >> z >> blockID;

            world.setBlock(x, y, z, blockID);
        }
    }
}

void Client::init() {
    std::cout << "running client" << std::endl;

    network.init();

    Window::create({1400, 800}, "Voxel Game", false);

    Renderer::init(true, 2048, 1400, 800);
    Renderer::settings.vignette = false;
    Renderer::settings.SSR = false;
    Renderer::setSun({normalize(vec3(-0.4f, -0.6f, -1.0f)), vec3(1.4f, 1.3f, 1.0f) * 3.0f});

    camera.position = vec3(3.0f, 62.0f, 3.0f);
    camera.direction = vec3(0.0f, 0.0f, 1.0f);

    BlockRegistry::init();

    lua.init();
    lua.state["api"]["registerBlock"] = BlockRegistry::registerBlockLua;
    lua.runScripts();

//    for (int i = 0; i < chunk.size(); i++) {
//        int c = chunk.get<int>(i);
//        std::cout << c << ", " << std::endl;
//    }

    world.init(Context::CLIENT);
    worldRenderer.init();

    int range = WORLD_SIZE;

    for (int x = 0; x < range; x++) {
        for (int z = 0; z < range; z++) {
            requestChunkFromServer(x, z);
        }
    }


    player.position = vec3(50, 200, 50);
}

void Client::update(float delta) {
    vec3 lastPosition = player.position;
    world.update(delta);
    player.update(camera, delta);

    receivePackets();

    camera.dimensions = {(float)Window::getWindowSize().x, (float)Window::getWindowSize().y};


    if (!(player.position == lastPosition)) {
        Packet p;
        uint16_t packetID = 2;

        p << packetID << player.position.x << player.position.y << player.position.z;

        network.sendPacket(p);
    }
}

void Client::render() {
    Window::begin();

    //frustum.updateCamPosition(camera);

    frustum.setupInternals(camera.fov, (float)Window::getWindowSize().x/(float)Window::getWindowSize().y, 0.1f, 1000.0f);
    //if (Input::isKeyDown(Input::KEY_LEFT_CONTROL))
        frustum.updateCamPosition(camera);

    //frustum.renderDebug();


    worldRenderer.render();


    for (auto const& x : playerPositions)
    {
        vec3 position = x.second;
        Renderer::debug.renderDebugAABB(position - vec3(1.0f), position + vec3(1.0f), vec3(1.0f, 0.0f, 0.0f));
    }

    //render scene and update window
    Renderer::flush(camera, frustum, true);

    Window::end();
}

void Client::run(bool &running) {
    init();

    while (Window::isOpen()) {
        static float deltaTime;
        static float lastFrameTime = Window::getTime();

        float currentFrameTime = Window::getTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;


        update(deltaTime);
        render();
    }

    std::cout << "Shutting down client" << std::endl;

    enet_deinitialize();
}