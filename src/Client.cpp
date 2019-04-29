#include "Client.hpp"
#include "NetworkManagerClient.hpp"
#include "Noise.hpp"

#include "core/Window.hpp"
#include "core/Renderer.hpp"
#include "core/Primitives.hpp"
#include "core/Input.hpp"
#include "core/GuiRenderer.hpp"
#include "core/AssimpFile.hpp"

#include <cstring>

#include <enet/enet.h>

#include "Packet.hpp"
#include "Block.hpp"

#include <typeinfo>
#include <typeindex>

#include <strstream>

Client::Client(): network(*this), worldRenderer(world), player(world, *this), itemRenderer(*this) {
    rleCache = new uint8_t[16*16*256*5];
}

Client::~Client() {
    delete[] rleCache;
}

void Client::manageChunks(vec2i playerChunkPosition) {
    auto chunks = world.getChunks();

    //chunk deletion
    for (auto i : chunks) {
        std::shared_ptr<Chunk> chunk = i.second;


        if (abs(playerChunkPosition.x - chunk->chunk_x) > settings.render_distance || abs(playerChunkPosition.y - chunk->chunk_z) > settings.render_distance) {
            world.deleteChunk(chunk->chunk_x, chunk->chunk_z);
            worldRenderer.deleteChunkRenderer(chunk->chunk_x, chunk->chunk_z);
        }
    }

    //chunk addition
    for (int x = -settings.render_distance; x < settings.render_distance+1; x++) {
        for (int z = -settings.render_distance; z < settings.render_distance+1; z++) {
            vec2i chunkPos = vec2i(x, z) + playerChunkPosition;

            if (!world.chunkExists(chunkPos.x, chunkPos.y)) {
                if (expectedChunks.find({chunkPos.x, chunkPos.y}) == expectedChunks.end()) {
                    requestChunkFromServer(chunkPos.x, chunkPos.y);

                    expectedChunks[{chunkPos.x, chunkPos.y}] = 1;
                }
            }

        }
    }
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

            expectedChunks.erase({x, z});
        }
        if (packetID == 4) {
            int x, y, z, blockID;

            p >> x >> y >> z >> blockID;

            world.setBlock(x, y, z, world.blockRegistry.getBlock(blockID));
        }
    }
}

void Client::init() {
    std::cout << "running client" << std::endl;

    settings.load("settings.json");

    Input::registerKeyPressedCallback([&] (int key) {
        lua.pushEvent("key_press", key);
    });

    Input::registerCharPressedCallback([&] (int key) {
        lua.pushEvent("char_press", (char)key);
    });

    Window::create({1400, 800}, "Cube Quest", false, settings.vsync);

    Renderer::init(1400*settings.resolution_scale, 800*settings.resolution_scale);
//    Renderer::settings.vignette = false;
//    Renderer::settings.SSR = false;
//
//    Renderer::settings.ssao = settings.fancy_graphics;
//    Renderer::settings.bloom = settings.fancy_graphics;
//    Renderer::settings.fxaa = settings.fancy_graphics;
//
//    Renderer::settings.fogOuter = settings.render_distance*16.0f;
//    Renderer::settings.fogInner = Renderer::settings.fogOuter * 0.8f;
//
//    Renderer::setSun({normalize(vec3(-0.4f, -0.6f, -1.0f)), vec3(1.4f, 1.3f, 1.0f) * 3.0f});

    camera.position = vec3(3.0f, 62.0f, 3.0f);
    camera.direction = vec3(0.0f, 0.0f, 1.0f);

    itemRenderer.init();

    lua.init();
    lua.addCommonFunctions(world);
    lua.addClientSideFunctions(*this);
    lua.runScripts();
    lua.runClientScripts();
}

void Client::connectToServer(std::string address, int port) {
    std::cout << "connecting to server" << std::endl;

    network.init(address, port);

    world.init(Context::CLIENT);
    worldRenderer.init();

    player.position = vec3(16*16, 250, 18*16);
    vec2i playerChunkPosition = vec2i((int)player.position.x >> 4, (int)player.position.z >> 4);
    manageChunks(playerChunkPosition);

    inGame = true;
}

void Client::connectToIntegratedServer() {
    integratedServer = new Server();

    serverThread = new std::thread([&]() {
        integratedServer->run(running, 1234);
    });

    connectToServer("localhost", 1234);
}

void Client::update(float delta) {
    vec3 lastPosition = player.position;
    vec2i lastPlayerChunkPosition = vec2i((int)player.position.x >> 4, (int)player.position.z >> 4);

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

    vec2i playerChunkPosition = vec2i((int)camera.position.x >> 4, (int)camera.position.z >> 4);

    if (!(playerChunkPosition == lastPlayerChunkPosition)) {
        manageChunks(playerChunkPosition);
    }
}

void Client::render() {
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
    Renderer::flush(camera);

    vec2 size = vec2((float)Window::getWindowSize().x, (float)Window::getWindowSize().y);

    lua.pushEvent("gui_ingame", size.x, size.y);
}

void Client::run() {
    init();

    while (Window::isOpen()) {
        Window::begin();

        if (inGame) {
            static float deltaTime;
            static float lastFrameTime = Window::getTime();

            float currentFrameTime = Window::getTime();
            deltaTime = currentFrameTime - lastFrameTime;
            lastFrameTime = currentFrameTime;


            update(deltaTime);

            render();


        }
        else {
            //main menu
            Renderer::flush(camera);

            vec2 size = vec2((float)Window::getWindowSize().x, (float)Window::getWindowSize().y);
            lua.pushEvent("gui_main_menu", size.x, size.y);
        }

        Window::end();
    }
    running = false;

    std::cout << "Shutting down client" << std::endl;

    enet_deinitialize();

    if (integratedServer) {
        serverThread->join();

        delete integratedServer;
        delete serverThread;
    }
}