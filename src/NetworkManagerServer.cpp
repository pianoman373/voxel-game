#include "NetworkManagerServer.hpp"
#include "NetworkManagerClient.hpp"

#include <iostream>
#include <thread>
#include <string>
//#include <unistd.h>


std::vector<ClientMessage> NetworkManagerServer::clientToServer;
std::mutex NetworkManagerServer::clientToServerMutex;
sf::UdpSocket NetworkManagerServer::socket;
bool NetworkManagerServer::isLocal = false;

std::vector<User> NetworkManagerServer::users;

User NetworkManagerServer::getUserByIp(sf::IpAddress ip, unsigned short port) {
    unsigned int integerIP = ip.toInteger();
    for (unsigned int i = 0; i < users.size(); i++) {
        User u = users[i];
        if (u.address.toInteger() == integerIP && u.port == port) {
            return u;
        }
    }
    return {sf::IpAddress(), 0, "null"};
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

        //usleep(100*1000);
        std::cout << "received packet" << std::endl;

        ClientMessage message;
        message.packet = packet;
        message.ip = sender;
        message.port = port;

        clientToServerMutex.lock();
        clientToServer.push_back(message);
        clientToServerMutex.unlock();
    }
}

void NetworkManagerServer::bind() {
    // bind the socket to a port
    if (isLocal) {

    }
    else {
        if (socket.bind(54000) != sf::Socket::Done)
        {
            // error...
        }

        std::cout << "bound server to port 54000" << std::endl;

        std::thread thread(handleIncomingPackets);
        thread.detach();
    }
}

void NetworkManagerServer::sendToAll(sf::Packet packet) {
    if (isLocal) {
        NetworkManagerClient::serverToClientMutex.lock();
        NetworkManagerClient::serverToClient.push_back(packet);
        NetworkManagerClient::serverToClientMutex.unlock();
    }
    else {
        for (unsigned int i = 0; i < users.size(); i++) {
            User currentUser = users[i];
            socket.send(packet, currentUser.address, currentUser.port);
        }
    }
}

void NetworkManagerServer::send(sf::Packet packet, sf::IpAddress recipient, unsigned short port) {
    socket.send(packet, recipient, port);
}

void NetworkManagerServer::handshake(sf::IpAddress sender, unsigned short port, std::string name) {
    if (isLocal) {

    }
    else {
        users.push_back({sender, port, name});

        int id = 0;
        sf::Packet replyPacket;
        replyPacket << id;

        socket.send(replyPacket, sender, port);
    }
}
