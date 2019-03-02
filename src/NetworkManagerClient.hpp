#pragma once

#include <enet/enet.h>
#include <vector>
#include "Packet.hpp"

class Client;

class NetworkManagerClient {
private:
    ENetHost *host;
    ENetPeer *peer;

    Client &client;

    std::vector<Packet> packetBuffer;

public:
    NetworkManagerClient(Client &client);

    void init(std::string address, int port);

    void processEvents();

    void sendPacket(Packet &packet);

    int getNumPackets();

    Packet &getPacket(int index);
};