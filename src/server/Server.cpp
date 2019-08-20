#include "server/Server.hpp"
#include "server/NetworkManagerServer.hpp"
#include "util/Noise.hpp"
#include "common/WorldGenerator.hpp"

#include <iostream>
#include <fstream>
#include <enet/enet.h>

#include <chrono>
#include <thread>

#include "optick.h"

#define WORLD_SIZE 128

Server::Server(): network(*this), world(*this, lua) {
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
    OPTICK_EVENT();
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

            if (x > 0 && z > 0 && x < WORLD_SIZE & z < WORLD_SIZE) {
                Packet returnPacket;
                returnPacket << packetID << x << z;

                std::shared_ptr<Chunk> c = world.getChunk(x, z);

                uint16_t length = c->serialize(rleCache);

                returnPacket << length;

                returnPacket.appendData(rleCache, length);

                network.sendPacket(p.senderID, returnPacket);
            }
        }
        if (packetID == 4) {
            int x, y, z, blockID;

            p >> x >> y >> z >> blockID;

            std::cout << "(Server) setblock at : " << x << ", " << y << ", " << z << std::endl;

            world.setBlock(x, y, z, world.blockRegistry.getBlock(blockID));

            
        }
    }
}

void Server::generateTerrain() {
    
}

void Server::init(int port) {
    network.init(port);

    world.setDeleteCallback([&] (std::shared_ptr<Chunk> chunk) {
        if (chunk->changedFromDisk) {
            std::cout << "saving chunk" << std::endl;

            chunkIO.saveChunk(chunk);
        }
    });

    lua.init();
    lua.addCommonFunctions(world);
    lua.runScripts();

    if (!isWorldSavePresent()) {
        WorldGenerator generator;

        generator.generate(lua, WORLD_SIZE);
    }
    

    std::cout << "loading chunks from disk..." << std::endl;

    for (int x = 0; x < WORLD_SIZE; x++) {
        for (int z = 0; z < WORLD_SIZE; z++) {
            chunkIO.loadChunk(world.getChunk(x, z));
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