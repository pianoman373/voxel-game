#include "Server.hpp"
#include "Common.hpp"

#include <iostream>
#include <SFML/Network.hpp>
#include <vector>

std::vector<sf::IpAddress> users;
std::vector<unsigned short> userPorts;

void Server::run() {
    Common::init();

    sf::UdpSocket socket;

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

        if (id == 0) {
            std::cout << "connected to " << sender << ":" << port << std::endl;
            users.push_back(sender);
            userPorts.push_back(port);
        }
        if (id == 1) {
            int x, y, z, blockID;

            packet >> x >> y >> z >> blockID;


            std::cout << "PlaceBlock(" << x << ", " << y << ", " << z << ", " << blockID << ")" << std::endl;

            sf::Packet replyPacket;
            replyPacket << x << y << z << blockID;


            for (unsigned int i = 0; i < users.size(); i++) {
                sf::IpAddress currentUser = users[i];
                unsigned short currentPort = userPorts[i];
                socket.send(replyPacket, currentUser, currentPort);
            }
        }
    }
}
