#pragma once

#include <enet/enet.h>
#include <vector>
#include <queue>

#include "util/Packet.hpp"

class Client;

class NetworkManagerClient {
private:
    ENetHost *host;
    ENetPeer *peer;

    Client &client;

    std::queue<Packet> packetBuffer;

public:
    NetworkManagerClient(Client &client);

    void init(std::string address, int port);

    void processEvents();

    void sendPacket(Packet &packet);

    int getNumPackets();

    Packet getPacket();

    bool isEmpty();
};