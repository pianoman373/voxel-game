#pragma once

#include <string>

class Server {
public:
    static void broadcastChat(std::string message);

    static void run();
};
