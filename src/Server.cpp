#include "Server.hpp"
#include "Common.hpp"
#include "NetworkManagerServer.hpp"

#include <iostream>
#include <vector>

void Server::broadcastChat(std::string message) {
    sf::Packet replyPacket;
    int id = 3;
    replyPacket << id;
    replyPacket << message;

    NetworkManagerServer::sendToAll(replyPacket);
}

void Server::run() {
    Common::init();
    Common::world.generate(false);

    NetworkManagerServer::bind();

    while (true) {

        //must check if the vector is empty cause somehow we end up getting garbage from size() sometimes
        NetworkManagerServer::clientToServerMutex.lock();
        if (!NetworkManagerServer::clientToServer.empty()) {
            for (int i = 0; i < NetworkManagerServer::clientToServer.size(); i++) {
                ClientMessage m = NetworkManagerServer::clientToServer[i];
                sf::Packet packet = m.packet;
                int userID = m.userID;
                int id;
                packet >> id;
                //User u = NetworkManagerServer::getUserByIp(sender, port);

                //disconnect pseudo-packet
                if (id == -1) {
                    std::cout << "got disconnect pseudo-packet" << std::endl;
                    broadcastChat(NetworkManagerServer::getUsernameByID(userID) + " left the game.");
                    NetworkManagerServer::removeUser(userID);

                    sf::Packet replyPacket;
                    replyPacket << id;
                    replyPacket << userID;

                    NetworkManagerServer::sendToAll(replyPacket);
                }
                //handshake
                if (id == 0) {
                    std::string name;
                    packet >> name;
                    std::cout << "connected to " << name << std::endl;

                    sf::Packet authPacket;
                    authPacket << id;
                    NetworkManagerServer::send(authPacket, userID);

                    sf::Packet replyPacket;
                    replyPacket << 3;
                    replyPacket << (name + std::string(" joined the game"));

                    NetworkManagerServer::addUsername(userID, name);
                    NetworkManagerServer::sendToAll(replyPacket);

                }
                //setblock
                if (id == 1) {
                    int x, y, z, blockID;

                    packet >> x >> y >> z >> blockID;


                    //std::cout << senderString << " -> " << "PlaceBlock(" << x << ", " << y << ", " << z << ", " << blockID << ")" << std::endl;

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

                    //std::string key = sender.toString() + std::string(":") + std::to_string(port);

                    sf::Packet replyPacket;
                    replyPacket << id;
                    replyPacket << x << y << z << userID;


                    NetworkManagerServer::sendToAll(replyPacket);
                }
                //chat message
                if (id == 3) {
                    std::string message;
                    packet >> message;

                    message = std::string("<") + NetworkManagerServer::getUsernameByID(userID) + std::string("> ") + message;

                    broadcastChat(message);
                }
                //get chunk
                if (id == 4) {
                    int x, y, z;
                    packet >> x >> y >> z;

                    Chunk *c = Common::world.getChunk(x, y, z);

                    bool isEmpty;
                    sf::Packet replyPacket;
                    replyPacket << id;
                    replyPacket << c->empty << x << y << z;


                    if (!c->empty) {
                        for (int i = 0; i < CHUNK_SIZE; i++) {
                            for (int j = 0; j < CHUNK_SIZE; j++) {
                                for (int k = 0; k < CHUNK_SIZE; k++) {
                                    char b = c->getBlock(i, j, k);
                                    replyPacket << (sf::Int8)b;
                                }
                            }
                        }
                    }

                    std::cout << "sending chunk" << std::endl;
                    NetworkManagerServer::send(replyPacket, userID);
                }
            }
            NetworkManagerServer::clientToServer.clear();
        }
        NetworkManagerServer::clientToServerMutex.unlock();
    }
}
