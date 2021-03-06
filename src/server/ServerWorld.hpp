#pragma once

#include "common/World.hpp"

class Server;

class ServerWorld : public World {
private:
    Server &server;

public:
    ServerWorld(Server &server, LuaHandler &lua);

    void setBlock(int x, int y, int z, Block &block);
};