#include "NetworkManagerClient.hpp"

#include "Client.hpp"

#include <iostream>

NetworkManagerClient::NetworkManagerClient(Client &client): client(client) {

}

void NetworkManagerClient::init(std::string address, int port) {
    ENetAddress eaddress;

    // b. Create a host using enet_host_create
    host = enet_host_create(NULL, 1, 2, 57600/8, 14400/8);

    if (host == NULL) {
        fprintf(stderr, "An error occured while trying to create an ENet server host\n");
    }

    enet_address_set_host(&eaddress, address.c_str());
    eaddress.port = port;

    // c. Connect and user service
    peer = enet_host_connect(host, &eaddress, 2, 0);

    if (peer == NULL) {
        fprintf(stderr, "No available peers for initializing an ENet connection");
    }

    ENetEvent event;
    // wait 10 minutes for connection to succeed
    if (enet_host_service(host, &event, 10*60000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
        // connection successful
        std::cout << "Connection to server succeeded" << std::endl;
        return;
    }
    // failure to connect
    std::cerr << "Connection to server failed" << std::endl;
}

void NetworkManagerClient::processEvents() {
    ENetEvent event;

    packetBuffer.clear();

    while (enet_host_service(host, &event, 0) > 0) {
        if (event.type == ENET_EVENT_TYPE_CONNECT) {
            printf("(Client) We got a new connection from %x\n", event.peer->address.host);
        }
        if (event.type == ENET_EVENT_TYPE_RECEIVE) {
            if (event.packet->dataLength > 0) {
                Packet p;
                p.appendData(event.packet->data, event.packet->dataLength);

                packetBuffer.push_back(p);
            }
        }
        if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
            printf("(Client) %s disconnected.\n", event.peer->data);

            // Reset client's information
            event.peer->data = NULL;
        }
    }
}

void NetworkManagerClient::sendPacket(Packet &packet) {
    ENetPacket *epacket = enet_packet_create(packet.getData(), packet.size(), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, epacket);
}

int NetworkManagerClient::getNumPackets() {
    return packetBuffer.size();
}

Packet& NetworkManagerClient::getPacket(int index) {
    return packetBuffer[index];
}