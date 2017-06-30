#pragma once

#include <SFML/Network.hpp>

class Server {
public:
    static sf::UdpSocket socket;

    static void run();
};
