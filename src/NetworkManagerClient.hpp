#pragma once

#include <vector>
#include <SFML/Network.hpp>

class NetworkManagerClient {
private:
    static std::vector<sf::Packet> clientToServer;
    static sf::UdpSocket socket;
    static sf::IpAddress connectedServer;
    static bool isLocal;

    static void handleIncomingPackets();
public:
    static std::vector<sf::Packet> serverToClient;

    static void connectToServer(sf::IpAddress remoteAddress);

    static void send(sf::Packet packet);
};
