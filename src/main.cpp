#include <iostream>
#include <thread>
#include <crucible/Window.hpp>
#include <crucible/Renderer.hpp>
#include <crucible/Camera.hpp>

#include "Server.hpp"
#include "Client.hpp"

#include "NetworkManagerClient.hpp"
#include "NetworkManagerServer.hpp"

int main(int argc, char *argv[]) {
    if (argc > 1 && std::string(argv[1]) == "-s") {
        std::cout << "running server" << std::endl;
        Server::run();
    }
    else if (argc > 3 && std::string(argv[1]) == "-c") {
        std::cout << "running game as client connecting to server: " << std::string(argv[3]) << " as " <<  std::string(argv[2]) << std::endl;
        Client::run(std::string(argv[2]), std::string(argv[3]));
    }
    else {
        std::cout << "running game locally" << std::endl;
        NetworkManagerClient::isLocal = true;
        NetworkManagerServer::isLocal = true;

        Client::run("Player", "localhost");
    }
}
