#pragma once

#include "NetworkManagerServer.hpp"
#include "World.hpp"
#include "LuaHandler.hpp"


class Server {
public:
    World world;
    NetworkManagerServer network;
    LuaHandler lua;
    uint8_t *rleCache;

    void update();

    void receivePackets();

    void generateTerrain(Chunk *c);

    void init();

    Server();

    ~Server();

    void run(bool &running);
};