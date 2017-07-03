#pragma once

#include "Window.hpp"

#include <SFML/Network.hpp>
#include <string>

/**
 * Controls mainly user-side and graphical tasks.
 */
class Client {
private:
    static void init();
public:
    static sf::UdpSocket socket;
    static sf::IpAddress connectedServer;
    static Window window;

    static void run(std::string ip);

    static void scrollBlocks(int direction);
};
