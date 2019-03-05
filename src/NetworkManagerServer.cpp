#include "NetworkManagerServer.hpp"

#include <iostream>

#include "Packet.hpp"

static std::string addressToString(const ENetAddress* address)
{
    uint8_t a = (uint8_t)(address->host);
    uint8_t b = (uint8_t)(address->host >> 8);
    uint8_t c = (uint8_t)(address->host >> 16);
    uint8_t d = (uint8_t)(address->host >> 24);
    return std::to_string(a) + "." + std::to_string(b) + "." + std::to_string(c) + "." + std::to_string(d) + ":" + std::to_string(address->port);
}

NetworkManagerServer::NetworkManagerServer(Server &server): server(server) {

}

void NetworkManagerServer::init(int port) {
    ENetAddress address;


    // b. Create a host using enet_host_create
    address.host = ENET_HOST_ANY;
    address.port = port;

    host = enet_host_create(&address, 32, 2, 0, 0);

    if (host == NULL) {
        fprintf(stderr, "An error occured while trying to create an ENet server host\n");
        return;
    }
}

void NetworkManagerServer::processEvents() {
    ENetEvent event;

    packetBuffer.clear();

    while (enet_host_service(host, &event, 0) > 0) {
        if (event.type == ENET_EVENT_TYPE_CONNECT) {
            std::string address = addressToString(&event.peer->address);
            printf("(Server) We got a new connection from %s (ID: %i)\n",
                   address.c_str(), event.peer->incomingPeerID);

            Packet p;
            uint16_t packetID = 0;
            uint16_t id = event.peer->incomingPeerID;
            p << packetID;
            p << id;

            packetBuffer.push_back(p);

            connectedPeers[event.peer->incomingPeerID] = event.peer;
        }
        if (event.type == ENET_EVENT_TYPE_RECEIVE) {
            if (event.packet->dataLength > 0) {
                Packet p;
                p.appendData(event.packet->data, event.packet->dataLength);
                p.senderID = event.peer->incomingPeerID;

                packetBuffer.push_back(p);
            }
        }
        if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
            printf("%s disconnected.\n", event.peer->data);

            connectedPeers.erase(event.peer->incomingPeerID);
        }
    }
}

ENetPeer* NetworkManagerServer::getPeer(uint16_t id) {
    auto iter = connectedPeers.find(id);
    if (iter == connectedPeers.end()) {
        return nullptr;
    }
    return iter->second;
}

void NetworkManagerServer::broadcastPacket(Packet &packet) {
    ENetPacket *epacket = enet_packet_create(packet.getData(), packet.size(), ENET_PACKET_FLAG_RELIABLE);
    enet_host_broadcast(host, 0, epacket);
}

void NetworkManagerServer::sendPacket(uint16_t peer, Packet &packet) {
    ENetPeer *recipient = getPeer(peer);

    if (recipient) {
        ENetPacket *epacket = enet_packet_create(packet.getData(), packet.size(), ENET_PACKET_FLAG_RELIABLE);

        enet_peer_send(recipient, 0, epacket);
    }
}

void NetworkManagerServer::broadcastPacketExcept(uint16_t peer, Packet &packet) {
    for (auto const &ref: connectedPeers) {
        if (ref.first != peer) {
            sendPacket(ref.first, packet);
        }
    }
}

int NetworkManagerServer::getNumPackets() {
    return packetBuffer.size();
}

Packet& NetworkManagerServer::getPacket(int index) {
    return packetBuffer[index];
}