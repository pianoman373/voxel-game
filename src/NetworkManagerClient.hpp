#pragma once

#include <deque>
#include <mutex>
#include <SFML/Network.hpp>

class NetworkManagerClient {
private:
    static sf::TcpSocket socket;
    static sf::IpAddress connectedServer;

    static void handleIncomingPackets();
public:
    static bool isLocal;
    static std::vector<sf::Packet> serverToClient;
    static std::mutex serverToClientMutex;

    static void connectToServer(std::string username, sf::IpAddress remoteAddress);

    static void send(sf::Packet packet);
};
