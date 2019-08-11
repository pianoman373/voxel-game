#include "server/ServerWorld.hpp"
#include "server/Server.hpp"

ServerWorld::ServerWorld(Server &server, LuaHandler &lua): World(lua), server(server) {

}

void ServerWorld::setBlock(int x, int y, int z, Block &block) {
    World::setBlock(x, y, z, block);

    Packet returnPacket;
    uint16_t packetID = 4;
    returnPacket << packetID << x << y << z << block.getID();

    server.network.broadcastPacket(returnPacket);

    block.onPlace(*this, x, y, z);
}