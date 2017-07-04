#include "Server.hpp"
#include "Common.hpp"
#include "NetworkManagerServer.hpp"

#include <iostream>
#include <vector>

void Server::run() {
    Common::init();

    NetworkManagerServer::bind();

    while (true) {

        //must check if the vector is empty cause somehow we end up getting garbage from size() sometimes
        if (!NetworkManagerServer::clientToServer.empty()) {
            for (int i = 0; i < NetworkManagerServer::clientToServer.size(); i++) {
                ClientMessage m = NetworkManagerServer::clientToServer[i];
                sf::Packet packet = m.packet;
                sf::IpAddress sender = m.ip;
                unsigned short port = m.port;
                int id;
                packet >> id;

                std::string senderString = sender.toString() + std::string(":") + std::to_string(port);

                if (id == 0) {
                    std::cout << "connected to " << sender << ":" << port << std::endl;
                    NetworkManagerServer::handshake(sender, port);
                }
                if (id == 1) {
                    int x, y, z, blockID;

                    packet >> x >> y >> z >> blockID;


                    std::cout << senderString << " -> " << "PlaceBlock(" << x << ", " << y << ", " << z << ", " << blockID << ")" << std::endl;

                    sf::Packet replyPacket;
                    replyPacket << id;
                    replyPacket << x << y << z << blockID;

                    NetworkManagerServer::sendToAll(replyPacket);
                }
                if (id == 2) {
                    float x, y, z;
                    packet >> x >> y >> z;

                    std::cout << senderString << " -> " << "CharacterMove(" << x << ", " << y << ", " << z << ")" << std::endl;

                    std::string key = sender.toString() + std::string(":") + std::to_string(port);

                    sf::Packet replyPacket;
                    replyPacket << id;
                    replyPacket << x << y << z << key;


                    NetworkManagerServer::sendToAll(replyPacket);
                }
                if (id == 3) {
                    std::string message;
                    packet >> message;

                    sf::Packet replyPacket;
                    replyPacket << id;
                    replyPacket << message;

                    NetworkManagerServer::sendToAll(replyPacket);
                }
            }
            NetworkManagerServer::clientToServer.clear();
        }
    }
}
