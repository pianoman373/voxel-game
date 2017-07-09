#include "NetworkManagerServer.hpp"
#include "NetworkManagerClient.hpp"

#include <iostream>
#include <thread>
#include <string>
//#include <unistd.h>


std::vector<ClientMessage> NetworkManagerServer::clientToServer;
std::mutex NetworkManagerServer::clientToServerMutex;
sf::TcpSocket NetworkManagerServer::socket;
sf::TcpListener NetworkManagerServer::listener;
bool NetworkManagerServer::isLocal = false;

sf::SocketSelector selector;

std::vector<sf::TcpSocket*> NetworkManagerServer::users;
std::mutex NetworkManagerServer::usersMutex;

User NetworkManagerServer::getUserByIp(sf::IpAddress ip, unsigned short port) {
//    unsigned int integerIP = ip.toInteger();
//    for (unsigned int i = 0; i < users.size(); i++) {
//        User u = users[i];
//        if (u.address.toInteger() == integerIP && u.port == port) {
//            return u;
//        }
//    }
    //return {sf::IpAddress(), 0, "null"};
}

void NetworkManagerServer::handleIncomingPackets() {
    listener.listen(55000);
    selector.add(listener);

    while (true) {
        selector.wait();

        if (selector.isReady(listener)) {
            sf::TcpSocket* client = new sf::TcpSocket;
            if (listener.accept(*client) == sf::Socket::Done) {
                usersMutex.lock();
                users.push_back(client);
                usersMutex.unlock();

                selector.add(*client);
                std::cout << "connected to client" << std::endl;
            }
            else {
                delete client;
                std::cout << "error connecting to client" << std::endl;
            }
        }
        else
        {
            usersMutex.lock();
            for (int i = 0; i < users.size(); i++)
            {
                sf::TcpSocket* socket = users[i];
                if (selector.isReady(*socket))
                {
                    // The client has sent some data, we can receive it
                    sf::Packet packet;
                    if (socket->receive(packet) == sf::Socket::Done)
                    {
                        std::cout << "received packet" << std::endl;
                        clientToServerMutex.lock();
                        clientToServer.push_back({packet, i});
                        clientToServerMutex.unlock();
                    }
                }

            }
            usersMutex.unlock();
        }
    }
}

void NetworkManagerServer::bind() {
    // bind the socket to a port
    if (isLocal) {

    }
    else {
        //std::thread thread(handleIncomingConnections);
        //thread.detach();

        std::thread thread(handleIncomingPackets);
        thread.detach();\
    }
}

void NetworkManagerServer::sendToAll(sf::Packet packet) {
    if (isLocal) {
        NetworkManagerClient::serverToClientMutex.lock();
        NetworkManagerClient::serverToClient.push_back(packet);
        NetworkManagerClient::serverToClientMutex.unlock();
    }
    else {
        for (unsigned int i = 0; i < users.size(); i++) {
            users[i]->send(packet);
        }
    }
}

void NetworkManagerServer::send(sf::Packet packet, int userID) {
    //socket.send(packet, recipient, port);
    users[userID]->send(packet);
}

void NetworkManagerServer::handshake(sf::IpAddress sender, unsigned short port, std::string name) {
    if (isLocal) {

    }
    else {
        //users.push_back({sender, port, name});

        int id = 0;
        sf::Packet replyPacket;
        replyPacket << id;

        //socket.send(replyPacket, sender, port);
    }
}
