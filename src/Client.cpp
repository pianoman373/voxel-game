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

            std::cout << "(Client) Received chunk at: " << x << ", " << z << std::endl;
            bytesReceived += length;
            //std::cout << "Received a total of " << bytesReceived << " bytes." << std::endl;

            std::shared_ptr<Chunk> chunk = world.getChunk(x, z);

            p.releaseData(rleCache, length);

            chunk->unSerialize(rleCache, length);

            chunk->isDirty = true;

            worldRenderer.getChunkRenderer(x, z);

            expectedChunks.erase({x, z});
        }
        if (packetID == 4) {
            int x, y, z, blockID;

            p >> x >> y >> z >> blockID;

            world.setBlock(x, y, z, blockID);
        }
    }
}

void Client::init(std::string address, int port) {
    std::cout << "running client" << std::endl;

    settings.load("settings.json");

    network.init(address, port);

    Window::create({1400, 800}, "Voxel Game", false, settings.vsync);

    Renderer::init(settings.shadows, settings.shadow_resolution, 1400*settings.resolution_scale, 800*settings.resolution_scale);
    Renderer::settings.vignette = false;
    Renderer::settings.SSR = false;

    Renderer::settings.ssao = settings.fancy_graphics;
    Renderer::settings.bloom = settings.fancy_graphics;
    Renderer::settings.fxaa = settings.fancy_graphics;

    Renderer::settings.fogOuter = settings.render_distance*16.0f;
    Renderer::settings.fogInner = Renderer::settings.fogOuter * 0.8f;

    Renderer::setSun({normalize(vec3(-0.4f, -0.6f, -1.0f)), vec3(1.4f, 1.3f, 1.0f) * 3.0f});

    camera.position = vec3(3.0f, 62.0f, 3.0f);
    camera.direction = vec3(0.0f, 0.0f, 1.0f);

    BlockRegistry::init();

    lua.init();
    lua.state["api"]["registerBlock"] = BlockRegistry::registerBlockLua;
    lua.runScripts();

    world.init(Context::CLIENT);
    worldRenderer.init();

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

    auto chunks = world.getChunks();

    vec2i playerPosition = vec2i((int)camera.position.x >> 4, (int)camera.position.z >> 4);

    int renderdistance = settings.render_distance;


    //chunk deletion
    for (auto i : chunks) {
        std::shared_ptr<Chunk> chunk = i.second;


        if (abs(playerPosition.x - chunk->chunk_x) > renderdistance || abs(playerPosition.y - chunk->chunk_z) > renderdistance) {
            world.deleteChunk(chunk->chunk_x, chunk->chunk_z);
            worldRenderer.deleteChunkRenderer(chunk->chunk_x, chunk->chunk_z);
        }
    }

    //chunk addition
    for (int x = -renderdistance; x < renderdistance+1; x++) {
        for (int z = -renderdistance; z < renderdistance+1; z++) {
            vec2i chunkPos = vec2i(x, z) + playerPosition;

            if (!world.chunkExists(chunkPos.x, chunkPos.y)) {
                if (expectedChunks.find({chunkPos.x, chunkPos.y}) == expectedChunks.end()) {
                    requestChunkFromServer(chunkPos.x, chunkPos.y);

                    expectedChunks[{chunkPos.x, chunkPos.y}] = 1;
                }
            }

        }
    }

}

void Client::render() {
    Window::begin();

    //frustum.updateCamPosition(camera);

    frustum.setupInternals(camera.fov, (float)Window::getWindowSize().x/(float)Window::getWindowSize().y, 0.1f, 1000.0f);
    //if (Input::isKeyDown(Input::KEY_LEFT_CONTROL))
        frustum.updateCamPosition(camera);

    //frustum.renderDebug();


    worldRenderer.render(camera);


    for (auto const& x : playerPositions)
    {
        vec3 position = x.second;
        Renderer::debug.renderDebugAABB(position - vec3(1.0f), position + vec3(1.0f), vec3(1.0f, 0.0f, 0.0f));
    }

    //render scene and update window
    Renderer::flush(camera, frustum, true);

    Window::end();
}

void Client::run(bool &running, std::string address, int port) {
    init(address, port);

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