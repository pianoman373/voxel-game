#pragma once

#include <vector>
#include <SFML/Network.hpp>

class NetworkManagerClient {
private:
    static sf::UdpSocket socket;
    static sf::IpAddress connectedServer;

    static void handleIncomingPackets();
public:
    static bool isLocal;
    static std::vector<sf::Packet> serverToClient;
    static std::vector<sf::Packet> clientToServer;

    static void connectToServer(sf::IpAddress remoteAddress);

    static void send(sf::Packet packet);
};
