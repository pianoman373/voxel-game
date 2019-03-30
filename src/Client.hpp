#pragma once

#include <crucible/Camera.hpp>
#include <crucible/Model.hpp>
#include <crucible/Material.hpp>
#include <crucible/Bone.hpp>

#include "NetworkManagerClient.hpp"
#include "World.hpp"
#include "WorldRenderer.hpp"
#include "LuaHandler.hpp"
#include "Settings.hpp"
#include "Registry.hpp"
#include "ItemRenderer.hpp"
#include "Server.hpp"

#include <crucible/Frustum.hpp>


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

public:
    NetworkManagerClient network;
    World world;
    WorldRenderer worldRenderer;
    Player player;
    Frustum frustum;
    Settings settings;
    Registry registry;
    LuaHandler lua;
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

    void render();

    void run();
};