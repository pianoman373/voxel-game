#include "NetworkManagerClient.hpp"
#include "NetworkManagerServer.hpp"
#include "Client.hpp"
#include "Server.hpp"

#include <thread>
#include <iostream>

std::vector<sf::Packet> NetworkManagerClient::clientToServer;
std::vector<sf::Packet> NetworkManagerClient::serverToClient;

bool NetworkManagerClient::isLocal = false;
sf::UdpSocket NetworkManagerClient::socket;
sf::IpAddress NetworkManagerClient::connectedServer;

void NetworkManagerClient::handleIncomingPackets() {
    while (true) {
        sf::Packet packet;

        sf::IpAddress sender;
        unsigned short port;

        if (socket.receive(packet, sender, port) != sf::Socket::Done)
        {
            // error...
        }

//        Message message = {packet, sender, port};
//        messageStack.push_back(message);
        serverToClient.push_back(packet);
    }
}

void NetworkManagerClient::connectToServer(sf::IpAddress remoteAddress) {
    if (isLocal) {

    }
    else {
        // bind the socket to a port
        if (socket.bind(socket.AnyPort) != sf::Socket::Done)
        {
            // error...
        }
        connectedServer = remoteAddress;

        //send connection packet
        sf::Packet connectionPacket;
        int id = 0;
        connectionPacket << id;
        NetworkManagerClient::send(connectionPacket);

        while (true) {
            sf::Packet packet;
            sf::IpAddress sender;
            unsigned short port;
            //wait for response handshake packet
            if (socket.receive(packet, sender, port) != sf::Socket::Done)
            {
                // error...
            }

            packet >> id;

            if (id == 0) {
                break;
            }
        }

        std::thread thread(NetworkManagerClient::handleIncomingPackets);
        thread.detach();
    }
}

void NetworkManagerClient::send(sf::Packet packet) {
    if (isLocal) {
        //clientToServer.push_back(packet);
        NetworkManagerServer::clientToServer.push_back({packet, sf::IpAddress(), 0});
    }
    else {
        socket.send(packet, connectedServer, 54000);
    }
}
