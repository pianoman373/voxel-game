#pragma once

#include <crucible/Camera.hpp>

#include "NetworkManagerClient.hpp"
#include "World.hpp"
#include "WorldRenderer.hpp"
#include "LuaHandler.hpp"

#include <crucible/Frustum.hpp>

#include <map>

class Client {
private:
    Camera camera;

    std::map<int, vec3> playerPositions;

    LuaHandler lua;

    uint8_t *rleCache;

public:
    NetworkManagerClient network;
    World world;
    WorldRenderer worldRenderer;
    Player player;
    Frustum frustum;

    Client();

    ~Client();

    void requestChunkFromServer(int x, int z);

    void receivePackets();

    void init();

    void update(float delta);

    void render();

    void run(bool &running);
};