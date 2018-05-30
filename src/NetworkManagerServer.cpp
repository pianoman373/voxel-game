#include "NetworkManagerServer.hpp"
#include "NetworkManagerClient.hpp"

#include <iostream>
#include <thread>

std::vector<ClientMessage> NetworkManagerServer::clientToServer;
std::mutex NetworkManagerServer::clientToServerMutex;
sf::TcpSocket NetworkManagerServer::socket;
sf::TcpListener NetworkManagerServer::listener;
bool NetworkManagerServer::isLocal = false;

sf::SocketSelector selector;

std::map<int, User> NetworkManagerServer::users;
std::mutex NetworkManagerServer::usersMutex;

int generateID() {
    static int i = -1;
    i++;
    return i;
}

std::string NetworkManagerServer::getUsernameByID(int id) {
    return users[id].name;
}

void NetworkManagerServer::addUsername(int id, std::string name) {
    users[id].name = name;
}

void NetworkManagerServer::handleIncomingPackets() {
    listener.listen(55000);
    selector.add(listener);

    while (true) {
        selector.wait();

        if (selector.isReady(listener)) {
            sf::TcpSocket* client = new sf::TcpSocket;
            if (listener.accept(*client) == sf::Socket::Done) {
                int userID = generateID();
                users[userID] = {client, ""};

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
            for (auto it = users.begin(); it != users.end(); ++it)
            {
                sf::TcpSocket* socket = it->second.socket;
                if (selector.isReady(*socket))
                {
                    // The client has sent some data, we can receive it
                    sf::Packet packet;
                    sf::TcpSocket::Status s = socket->receive(packet);
                    if (s == sf::Socket::Done)
                    {
                        //std::cout << "received packet" << std::endl;
                        clientToServerMutex.lock();
                        clientToServer.push_back({packet, it->first});
                        clientToServerMutex.unlock();
                    }
                    else if (s == sf::Socket::Disconnected) {
                        sf::Packet p;
                        int id = -1;
                        int userID = it->first;
                        p << id ;

                        clientToServerMutex.lock();
                        clientToServer.push_back({p, userID});
                        clientToServerMutex.unlock();

                        selector.remove(*socket);
                        socket->disconnect();
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
        for (auto it = users.begin(); it != users.end(); ++it) {
            it->second.socket->send(packet);
        }
    }
}

void NetworkManagerServer::send(sf::Packet packet, int userID) {
    //socket.send(packet, recipient, port);
    users[userID].socket->send(packet);
}

void NetworkManagerServer::removeUser(int userID) {
    usersMutex.lock();
    users.erase(userID);
    usersMutex.unlock();
}
