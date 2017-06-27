#include "Server.hpp"
#include "Common.hpp"

#include <iostream>
#include <SFML/Network.hpp>

void Server::run() {
    Common::init();

    sf::UdpSocket socket;

    // bind the socket to a port
    if (socket.bind(54000) != sf::Socket::Done)
    {
        // error...
    }

    std::cout << "bound server to port 5400" << std::endl;

    while (true) {
        char data[100];
        std::size_t received;

        sf::IpAddress sender;
        unsigned short port;
        if (socket.receive(data, 100, received, sender, port) != sf::Socket::Done)
        {
            // error...
        }
        std::cout << "Received " << received << " bytes from " << sender << " on port " << port << std::endl;
    }
}
