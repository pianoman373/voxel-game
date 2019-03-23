#include "Server.hpp"
#include "NetworkManagerServer.hpp"
#include "Noise.hpp"

#include <iostream>
#include <fstream>
#include <enet/enet.h>

#include <chrono>
#include <thread>


Server::Server(): network(*this) {
    rleCache = new uint8_t[16*16*256*5];
}

Server::~Server() {
    delete[] rleCache;
}

bool Server::isWorldSavePresent() {
    std::ifstream i("save/info.json");
    json j;

    if (i) {
        i >> j;

        return j["generated"];
    }

    return false;
}

void Server::update() {
    receivePackets();
}

void Server::receivePackets() {
    network.processEvents();

    for (int i = 0; i < network.getNumPackets(); i++) {
        Packet &p = network.getPacket(i);

        uint16_t packetID;
        p >> packetID;

        if (packetID == 0) {
            std::cout << "connection event!" << std::endl;
        }
        if (packetID == 1) {
            std::string s;
            p >> s;

            std::cout << "(Server) Message from client : " << s << std::endl;

            network.broadcastPacket(p);
        }
        if (packetID == 2) { //player position data
            float x, y, z;

            p >> x >> y >> z;

            //std::cout << "(Server) received position data " << x << ", " << y << ", " << z << std::endl;

            Packet returnPacket;
            returnPacket << packetID << p.senderID << x << y << z;

            //network.broadcastPacket(returnPacket);
            network.broadcastPacketExcept(p.senderID, returnPacket);
        }
        if (packetID == 3) {
            int x, z;

            p >> x >> z;

            //std::cout << "(Server) Received chunk request at: " << x << ", " << z << std::endl;

            Packet returnPacket;
            returnPacket << packetID << x << z;

            std::shared_ptr<Chunk> c = world.getChunk(x, z);

            uint16_t length = c->serialize(rleCache);

            returnPacket << length;

            returnPacket.appendData(rleCache, length);

            network.sendPacket(p.senderID, returnPacket);
        }
        if (packetID == 4) {
            int x, y, z, blockID;

            p >> x >> y >> z >> blockID;

            std::cout << "(Server) setblock at : " << x << ", " << y << ", " << z << std::endl;

            world.setBlock(x, y, z, world.blockRegistry.getBlock(blockID));

            Packet returnPacket;
            returnPacket << packetID << x << y << z << blockID;

            network.broadcastPacketExcept(p.senderID, returnPacket);
        }
    }
}

void Server::generateTerrain() {
    auto start = std::chrono::high_resolution_clock::now();

    int progress = 0;
    int i = 0;

    int worldSize = 32;

    std::cout << "generating terrain..." << std::endl;

    for (int x = 0; x < worldSize; x++) {
        for (int z = 0; z < worldSize; z++) {
            sol::protected_function f(lua.state["generateWorld"]);
            sol::protected_function_result result = f(x, z, world.getChunk(x, z));
            if (result.valid()) {

            }
            else {
                // Call failed
                sol::error err = result;
                std::cout << err.what() << std::endl;
            }

            i++;

            int currentProgress = (int)(((float)i / ((float)worldSize*(float)worldSize)) * 100.0f);

            if (currentProgress != progress) {
                progress = currentProgress;

                std::cout << progress << "% complete." << std::endl;
            }
        }
    }

    auto finish = std::chrono::high_resolution_clock::now();
    float seconds = std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count() / 1000.0f;

    std::cout << "completed in " << seconds << "s " << std::endl;

    std::cout << "generating lighting..." << std::endl;

    //generate lighting
    for (int x = 0; x < 32; x++) {
        for (int z = 0; z < 32; z++) {
            world.getChunk(x, z)->calculateSunLighting();
        }
    }
}

void Server::init(int port) {
    network.init(port);

    world.init(Context::SERVER);
    world.setDeleteCallback([&] (std::shared_ptr<Chunk> chunk) {
        if (chunk->changedFromDisk) {
            std::cout << "saving chunk" << std::endl;

            chunkIO.saveChunk(chunk);
        }
    });

    lua.init();
    lua.addCommonFunctions(world);
    lua.runScripts();
    lua.state.script_file("mods/base/worldgen.lua", sol::script_default_on_error);

    lua.state.new_usertype<Chunk>( "Chunk",
        // typical member function that returns a variable
        "setBlockRaw", &Chunk::setBlockRaw
    );

    if (isWorldSavePresent()) {
        std::cout << "loading chunks from disk..." << std::endl;

        for (int x = 0; x < 32; x++) {
            for (int z = 0; z < 32; z++) {
                chunkIO.loadChunk(world.getChunk(x, z));
            }
        }
    }
    else {
        generateTerrain();

        for (int x = 0; x < 32; x++) {
            for (int z = 0; z < 32; z++) {
                chunkIO.saveChunk(world.getChunk(x, z));
            }
        }


        std::ofstream i("save/info.json");
        json j;

        j["generated"] = true;

        if (i) {
            i << j;
        }
    }

    std::cout << "complete!" << std::endl;
}

void Server::run(bool &running, int port) {
    std::cout << "running server" << std::endl;

    init(port);

    while (running) {
        update();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::cout << "Shutting down server" << std::endl;

    world.deleteAllChunks();

    std::cout << "shutdown complete" << std::endl;

}