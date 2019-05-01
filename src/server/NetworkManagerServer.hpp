#pragma once

#include <enet/enet.h>

#include <map>

#include "util/Packet.hpp"

class Server;

class NetworkManagerServer {
private:
    ENetHost *host;

    Server &server;

    std::vector<Packet> packetBuffer;

    std::map<uint16_t, ENetPeer*> connectedPeers;

public:
    NetworkManagerServer(Server &server);

    void init(int port);

    void processEvents();

    ENetPeer* getPeer(uint16_t id);

    void broadcastPacket(Packet &packet);

    void sendPacket(uint16_t peer, Packet &packet);

    void broadcastPacketExcept(uint16_t peer, Packet &packet);

    int getNumPackets();

    Packet &getPacket(int index);
};