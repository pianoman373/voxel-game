#include <iostream>
#include <enet/enet.h>

#include "Client.hpp"
#include "Server.hpp"

#include "Noise.hpp"
#include "SimplexNoise.hpp"

#include <thread>

Server *s;
Client *c;

#ifdef WIN32
#define FFI extern "C" __declspec(dllexport)
#else
#define FFI extern "C"
#endif

FFI void setChunk(int x, int z, uint8_t *data) {
    memcpy(s->world.getChunk(x, z)->blocks, data, 16*16*256);
}

FFI float ridgedNoise(float x, float y, int octaves, float frequency, float persistence) {
    return Noise::ridgedNoise(x, y, octaves, frequency, persistence);
}

FFI float noise3D(float x, float y, float z) {
    return SimplexNoise::noise(x, y, z);
}

int main(int argc, char *argv[]) {
    bool running = true;
    s = new Server();
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
