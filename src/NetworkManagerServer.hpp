#pragma once

#include <vector>
#include <mutex>
#include <SFML/Network.hpp>

struct ClientMessage {
    sf::Packet packet;
    int userID;
};

struct User {
    sf::TcpSocket *socket;
    std::string name;
};

class NetworkManagerServer {
private:
    static sf::TcpSocket socket;
    static sf::TcpListener listener;

    static std::vector<sf::TcpSocket*> users;
    static std::mutex usersMutex;

    static std::vector<std::string> usernames;

    static void handleIncomingPackets();

public:
    static bool isLocal;
    static std::vector<ClientMessage> clientToServer;
    static std::mutex clientToServerMutex;

    static std::string getUsernameByID(int id);

    static void addUsername(int id, std::string name);

    static void bind();

    static void sendToAll(sf::Packet packet);

    static void send(sf::Packet packet, int userID);

    static void handshake(sf::IpAddress sender, unsigned short port, std::string name);
};
