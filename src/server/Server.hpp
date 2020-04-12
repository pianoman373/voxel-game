#pragma once

#include "server/NetworkManagerServer.hpp"
#include "common/LuaHandler.hpp"
#include "server/ChunkIO.hpp"
#include "server/ServerWorld.hpp"


class Server {
public:
    ServerWorld world;
    NetworkManagerServer network;
    LuaHandler lua;
    uint8_t *rleCache;
    ChunkIO chunkIO;
    std::string folder;

    bool isWorldSavePresent();

    void update();

    void receivePackets();

    void generateTerrain();

    void init(int port);

    Server(const std::string &folder);

    ~Server();

    void run(bool &running, int port);
};