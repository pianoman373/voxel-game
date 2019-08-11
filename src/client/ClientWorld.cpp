#include "client/ClientWorld.hpp"
#include "client/Client.hpp"

ClientWorld::ClientWorld(Client &client, LuaHandler &lua): World(lua), client(client){

}

void ClientWorld::setBlock(int x, int y, int z, Block &block) {
    World::setBlock(x, y, z, block);

    Packet p;
    uint16_t packetID = 4;
    int blockID = 0;

    p << packetID << x << y << z << block.getID();

    client.network.sendPacket(p);
}

void ClientWorld::setBlockNoPacket(int x, int y, int z, Block &block) {
    World::setBlock(x, y, z, block);
}