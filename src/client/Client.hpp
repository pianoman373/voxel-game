#pragma once

#include "rendering/Camera.hpp"
#include "rendering/Material.hpp"

#include "client/NetworkManagerClient.hpp"
#include "client/ClientWorld.hpp"
#include "client/WorldRenderer.hpp"
#include "common/LuaHandler.hpp"
#include "util/Settings.hpp"
#include "util/Registry.hpp"
#include "client/ItemRenderer.hpp"
#include "server/Server.hpp"

#include "rendering/Frustum.hpp"


#include <map>

class Client {
private:
    Camera camera;

    std::map<int, vec3> playerPositions;

    std::unordered_map<vec2i, int> expectedChunks;

    bool inGame = false;

    uint8_t *rleCache;

    Server *integratedServer = nullptr;
    std::thread *serverThread;

    bool running = true;
    bool abortNetworkProcessing = false;

public:
    LuaHandler lua;
    NetworkManagerClient network;
    ClientWorld world;
    WorldRenderer worldRenderer;
    Player *player = nullptr;
    Frustum frustum;
    Settings settings;
    Registry registry;
    
    ItemRenderer itemRenderer;

    Client();

    ~Client();

    void manageChunks(vec2i playerChunkPosition);

    void requestChunkFromServer(int x, int z);

    void receivePackets();

    void init();

    void connectToServer(std::string address, int port);

    void connectToIntegratedServer();

    void update(float delta);

    void tick();

    void render();

    void run();
};