#pragma once

#include "NetworkManagerServer.hpp"
#include "World.hpp"
#include "LuaHandler.hpp"
#include "ChunkIO.hpp"


class Server {
public:
    World world;
    NetworkManagerServer network;
    LuaHandler lua;
    uint8_t *rleCache;
    ChunkIO chunkIO;

    bool isWorldSavePresent();

    void update();

    void receivePackets();

    void generateTerrain();

    void init(int port);

    Server();

    ~Server();

    void run(bool &running, int port);
};