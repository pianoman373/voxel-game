#include <iostream>
#include <enet/enet.h>

#include "client/Client.hpp"
#include "server/Server.hpp"

#include "util/Noise.hpp"
#include "util/SimplexNoise.hpp"

#include <thread>

Server *s;
Client *c;

#ifdef WIN32
#define FFI extern "C" __declspec(dllexport)
#else
#define FFI extern "C"
#endif

FFI unsigned char* getChunkData(void *c) {
    Chunk* object_pointer = *static_cast<Chunk**>(c);
    return object_pointer->blocks;
}

int main(int argc, char *argv[]) {
    bool running = true;
    s = new Server("serverWorld");
    c = new Client();

    // a. Initialize enet
    if (enet_initialize() != 0) {
        fprintf(stderr, "An error occured while initializing ENet.\n");
        return 0;
    }

    atexit(enet_deinitialize);

    if (argc > 1 && std::string(argv[1]) == "-s") {
        if (argc > 2) {
            int port = atoi(argv[2]);

            s->run(running, port);

            delete s;
        }
        else {
            std::cout << "Usage: voxel-game -s port" << std::endl;
        }
    }
    else {
        c->run();

        delete c;
    }
}
