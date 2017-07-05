#pragma once

#include <vector>
#include <SFML/Network.hpp>

struct ClientMessage {
    sf::Packet packet;
    sf::IpAddress ip;
    unsigned short port;
};

class NetworkManagerServer {
private:
    static sf::UdpSocket socket;

    static std::vector<sf::IpAddress> users;
    static std::vector<unsigned short> userPorts;

    static void handleIncomingPackets();

public:
    static bool isLocal;
    static std::vector<ClientMessage> clientToServer;

    static void bind();

    static void sendToAll(sf::Packet packet);

    static void send(sf::Packet packet, sf::IpAddress recipient, unsigned short port);

    static void handshake(sf::IpAddress sender, unsigned short port);
};
