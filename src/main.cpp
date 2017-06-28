#include <iostream>

#include "Server.hpp"
#include "Client.hpp"

int main(int argc, char *argv[]) {
    //std::cout << argv[1] << std::endl;
    if (argc > 1 && std::string(argv[1]) == "-s") {
        std::cout << "running server" << std::endl;
        Server::run();
    }
    else {
        Client::run();
        return 1;
    }
}
