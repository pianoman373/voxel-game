#pragma once

#include "common/World.hpp"

class Client;

class ClientWorld : public World {
private:
    Client &client;

public:
    ClientWorld(Client &client, LuaHandler &lua);

    void setBlock(int x, int y, int z, Block &block);

    void setBlockNoPacket(int x, int y, int z, Block &block);
};