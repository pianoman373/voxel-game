#include "NetworkManagerClient.hpp"
#include "NetworkManagerServer.hpp"
#include "Client.hpp"
#include "Server.hpp"

#include <thread>
#include <iostream>
#include <string>

std::vector<sf::Packet> NetworkManagerClient::serverToClient;
std::mutex NetworkManagerClient::serverToClientMutex;

bool NetworkManagerClient::isLocal = false;
sf::TcpSocket NetworkManagerClient::socket;
sf::IpAddress NetworkManagerClient::connectedServer;

void NetworkManagerClient::handleIncomingPackets() {
    static int i = 0;
    while (true) {
        sf::Packet packet;

        sf::IpAddress sender;
        unsigned short port;

        if (socket.receive(packet) != sf::Socket::Done)
        {
            // error...
            std::cout << "error on packet receive" << std::endl;
        }
        //std::cout << "received packet " << i << std::endl;
        i++;

        //usleep(100*1000);

//        Message message = {packet, sender, port};
//        messageStack.push_back(message);
        serverToClientMutex.lock();
        serverToClient.push_back(packet);
        serverToClientMutex.unlock();
    }
}

void NetworkManagerClient::connectToServer(std::string username, sf::IpAddress remoteAddress) {
    if (isLocal) {

    }
    else {
        sf::Socket::Status status = socket.connect(remoteAddress, 55000);
        if (status != sf::Socket::Done)
        {
            // error...
            std::cout << "error connecting to remote host" << std::endl;
            Client::shutdown();
        }

        connectedServer = remoteAddress;
        std::cout << "connected to server" << std::endl;

        //send connection packet
        sf::Packet connectionPacket;

        int id = 0;
        connectionPacket << id;
        connectionPacket << username;
        NetworkManagerClient::send(connectionPacket);

        while (true) {
            sf::Packet packet;
            sf::IpAddress sender;
            unsigned short port;
            //wait for response handshake packet
            if (socket.receive(packet) != sf::Socket::Done)
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
        NetworkManagerServer::clientToServerMutex.lock();
        NetworkManagerServer::clientToServer.push_back({packet, 0});
        NetworkManagerServer::clientToServerMutex.unlock();
    }
    else {
        socket.send(packet);
    }
}

void NetworkManagerClient::disconnect() {
    socket.disconnect();
}
