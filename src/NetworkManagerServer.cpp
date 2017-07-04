#include "NetworkManagerServer.hpp"

#include <iostream>
#include <thread>


std::vector<ClientMessage> NetworkManagerServer::clientToServer;
sf::UdpSocket NetworkManagerServer::socket;
bool NetworkManagerServer::isLocal = false;


std::vector<sf::IpAddress> NetworkManagerServer::users;
std::vector<unsigned short> NetworkManagerServer::userPorts;

static int generateID() {
    static int userID = 0;
    userID++;
    return userID;;
}

void NetworkManagerServer::handleIncomingPackets() {
    while (true) {
        sf::Packet packet;
        sf::IpAddress sender;
        unsigned short port;

        if (socket.receive(packet, sender, port) != sf::Socket::Done)
        {
            // error...
            std::cout << "error receiving packet" << std::endl;
        }
        std::cout << "received packet" << std::endl;

        ClientMessage message;
        message.packet = packet;
        message.ip = sender;
        message.port = port;

        clientToServer.push_back(message);


    }
}

void NetworkManagerServer::bind() {
    // bind the socket to a port
    if (socket.bind(54000) != sf::Socket::Done)
    {
        // error...
    }

    std::cout << "bound server to port 54000" << std::endl;

    std::thread thread(handleIncomingPackets);
    thread.detach();
}

void NetworkManagerServer::sendToAll(sf::Packet packet) {
    for (unsigned int i = 0; i < users.size(); i++) {
        sf::IpAddress currentUser = users[i];
        unsigned short currentPort = userPorts[i];
        socket.send(packet, currentUser, currentPort);
    }
}

void NetworkManagerServer::handshake(sf::IpAddress sender, unsigned short port) {
    users.push_back(sender);
    userPorts.push_back(port);

    int userID = generateID();
    int id = 0;
    sf::Packet replyPacket;
    replyPacket << 0;
    replyPacket << userID;

    socket.send(replyPacket, sender, port);
}
