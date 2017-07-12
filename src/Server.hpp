#pragma once

#include <SFML/Network.hpp>

class Server {
public:
    static void broadcastChat(std::string message);

    static void run();
};
