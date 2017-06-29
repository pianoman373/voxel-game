#include <iostream>
#include <thread>

#include "Server.hpp"
#include "Client.hpp"

int main(int argc, char *argv[]) {
    if (argc > 1 && std::string(argv[1]) == "-s") {
        std::cout << "running server" << std::endl;
        Server::run();
    }
    else if (argc > 2 && std::string(argv[1]) == "-c") {
        std::cout << "running game as client connecting to server: " << std::string(argv[2]) << std::endl;
        Client::run(std::string(argv[2]));
    }
    else {
        std::cout << "running game locally" << std::endl;
        Client::run("localhost");
    }
}
