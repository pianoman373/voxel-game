#include "Server.hpp"
#include "Common.hpp"
#include "NetworkManagerServer.hpp"

#include <iostream>
#include <vector>

void Server::run() {
    Common::init();
    Common::world.generate(false);

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
                User u = NetworkManagerServer::getUserByIp(sender, port);

                std::string senderString = sender.toString() + std::string(":") + std::to_string(port);

                //handshake
                if (id == 0) {
                    std::string name;
                    packet >> name;
                    NetworkManagerServer::handshake(sender, port, name);

                    std::cout << "connected to " << name  << " (" << sender << ":" << port << ")" << std::endl;

                    sf::Packet replyPacket;
                    replyPacket << 3;
                    replyPacket << (name + std::string(" joined the game"));

                    NetworkManagerServer::sendToAll(replyPacket);

                }
                //setblock
                if (id == 1) {
                    int x, y, z, blockID;

                    packet >> x >> y >> z >> blockID;


                    std::cout << senderString << " -> " << "PlaceBlock(" << x << ", " << y << ", " << z << ", " << blockID << ")" << std::endl;

                    Common::world.setBlock(x, y, z, blockID);

                    sf::Packet replyPacket;
                    replyPacket << id;
                    replyPacket << x << y << z << blockID;

                    NetworkManagerServer::sendToAll(replyPacket);
                }
                //character move
                if (id == 2) {
                    float x, y, z;
                    packet >> x >> y >> z;

                    //std::cout << senderString << " -> " << "CharacterMove(" << x << ", " << y << ", " << z << ")" << std::endl;

                    std::string key = sender.toString() + std::string(":") + std::to_string(port);

                    sf::Packet replyPacket;
                    replyPacket << id;
                    replyPacket << x << y << z << key;


                    NetworkManagerServer::sendToAll(replyPacket);
                }
                //chat message
                if (id == 3) {
                    std::string message;
                    packet >> message;

                    message = std::string("<") + u.name + std::string("> ") + message;

                    sf::Packet replyPacket;
                    replyPacket << id;
                    replyPacket << message;

                    NetworkManagerServer::sendToAll(replyPacket);
                }
                //get chunk
                if (id == 4) {
                    static int i = 0;
                    int x, y, z;
                    packet >> x >> y >> z;

                    sf::Packet replyPacket;
                    replyPacket << id;
                    replyPacket << x << y << z;

                    Chunk *c = Common::world.getChunk(x, y, z);
                    for (int i = 0; i < CHUNK_SIZE; i++) {
                        for (int j = 0; j < CHUNK_SIZE; j++) {
                            for (int k = 0; k < CHUNK_SIZE; k++) {
                                char b = c->getBlock(i, j, k);
                                replyPacket << (sf::Int8)b;
                            }
                        }
                    }

                    std::cout << "sending chunk: " << i << std::endl;
                    i++;
                    NetworkManagerServer::send(replyPacket, sender, port);
                }
            }
            NetworkManagerServer::clientToServer.clear();
        }
    }
}
