#pragma once

#include "Window.hpp"
#include "Frustum.hpp"

#include <SFML/Network.hpp>
#include <string>

/**
 * Controls mainly user-side and graphical tasks.
 */
class Client {
private:
    static void init();
public:
    static Frustum frustum;
    static sf::UdpSocket socket;
    static sf::IpAddress connectedServer;
    static Window window;

    static void run(std::string username, std::string ip);

    static void scrollBlocks(int direction);

    static void shutdown();
};
