#pragma once

#include <vector>
#include <mutex>
#include <SFML/Network.hpp>

struct ClientMessage {
    sf::Packet packet;
    sf::IpAddress ip;
    unsigned short port;
};

struct User {
    sf::IpAddress address;
    unsigned short port;
    std::string name;
};

class NetworkManagerServer {
private:
    static sf::UdpSocket socket;

    static std::vector<User> users;

    static void handleIncomingPackets();

public:
    static bool isLocal;
    static std::vector<ClientMessage> clientToServer;
    static std::mutex clientToServerMutex;

    static User getUserByIp(sf::IpAddress ip, unsigned short port);

    static void bind();

    static void sendToAll(sf::Packet packet);

    static void send(sf::Packet packet, sf::IpAddress recipient, unsigned short port);

    static void handshake(sf::IpAddress sender, unsigned short port, std::string name);
};
