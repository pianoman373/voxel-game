#pragma once

#include <SFML/Network.hpp>
#include <string>

/**
 * Controls mainly user-side and graphical tasks.
 */
class Client {
private:
    static void init();

    static void handleIncomingPackets();
public:
    static sf::UdpSocket socket;
    static sf::IpAddress connectedServer;

    static void run(std::string ip);
};
