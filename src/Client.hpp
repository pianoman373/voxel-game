#pragma once

#include <crucible/Camera.hpp>

#include "NetworkManagerClient.hpp"
#include "World.hpp"
#include "WorldRenderer.hpp"
#include "LuaHandler.hpp"
#include "Settings.hpp"

#include <crucible/Frustum.hpp>


#include <map>

class Client {
private:
    Camera camera;

    std::map<int, vec3> playerPositions;

    std::unordered_map<vec2i, int> expectedChunks;

    LuaHandler lua;

    uint8_t *rleCache;

public:
    NetworkManagerClient network;
    World world;
    WorldRenderer worldRenderer;
    Player player;
    Frustum frustum;
    Settings settings;

    Client();

    ~Client();

    void requestChunkFromServer(int x, int z);

    void receivePackets();

    void init(std::string address, int port);

    void update(float delta);

    void render();

    void run(bool &running, std::string address, int port);
};