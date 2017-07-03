#include "Server.hpp"
#include "Common.hpp"

#include <iostream>
#include <vector>

std::vector<sf::IpAddress> users;
std::vector<unsigned short> userPorts;

sf::UdpSocket Server::socket;

int generateID() {
    static int userID = 0;
    userID++;
    return userID;;
}

void Server::run() {
    Common::init();


    // bind the socket to a port
    if (socket.bind(54000) != sf::Socket::Done)
    {
        // error...
    }

    std::cout << "bound server to port 54000" << std::endl;

    while (true) {
        sf::Packet packet;
        sf::IpAddress sender;
        unsigned short port;
        if (socket.receive(packet, sender, port) != sf::Socket::Done)
        {
            // error...
        }
        //std::cout << "Received " << received << " bytes from " << sender << " on port " << port << std::endl;

        int id;
        packet >> id;

        std::string senderString = sender.toString() + std::to_string(port);

        if (id == 0) {
            std::cout << "connected to " << sender << ":" << port << std::endl;
            users.push_back(sender);
            userPorts.push_back(port);

            int userID = generateID();
            sf::Packet replyPacket;
            replyPacket << id;
            replyPacket << userID;

            socket.send(replyPacket, sender, port);
        }
        if (id == 1) {
            int x, y, z, blockID;

            packet >> x >> y >> z >> blockID;


            std::cout << senderString << " -> " << "PlaceBlock(" << x << ", " << y << ", " << z << ", " << blockID << ")" << std::endl;

            sf::Packet replyPacket;
            replyPacket << id;
            replyPacket << x << y << z << blockID;


            for (unsigned int i = 0; i < users.size(); i++) {
                sf::IpAddress currentUser = users[i];
                unsigned short currentPort = userPorts[i];
                socket.send(replyPacket, currentUser, currentPort);
            }
        }
        if (id == 2) {
            float x, y, z;
            packet >> x >> y >> z;

            std::cout << senderString << " -> " << "CharacterMove(" << x << ", " << y << ", " << z << ")" << std::endl;

            std::string key = sender.toString() + std::string(":") + std::to_string(port);

            sf::Packet replyPacket;
            replyPacket << id;
            replyPacket << x << y << z << key;


            for (unsigned int i = 0; i < users.size(); i++) {
                sf::IpAddress currentUser = users[i];
                unsigned short currentPort = userPorts[i];
                socket.send(replyPacket, currentUser, currentPort);
            }
        }
        if (id == 3) {
            std::string message;
            packet >> message;

            sf::Packet replyPacket;
            replyPacket << id;
            replyPacket << message;

            for (unsigned int i = 0; i < users.size(); i++) {
                sf::IpAddress currentUser = users[i];
                unsigned short currentPort = userPorts[i];
                socket.send(replyPacket, currentUser, currentPort);
            }
        }
    }
}
