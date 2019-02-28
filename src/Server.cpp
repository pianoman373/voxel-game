#include "Server.hpp"
#include "NetworkManagerServer.hpp"
#include "Noise.hpp"

#include <iostream>
#include <enet/enet.h>

#include <chrono>


Server::Server(): network(*this) {
    rleCache = new uint8_t[16*16*256*2];
}

Server::~Server() {
    delete[] rleCache;
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

            world.setBlock(x, y, z, blockID);

            Packet returnPacket;
            returnPacket << packetID << x << y << z << blockID;

            network.broadcastPacketExcept(p.senderID, returnPacket);
        }
    }
}

static float getHeight(int x, int z) {
    return Noise::ridgedNoise(x / 10.0f, z / 10.0f, 5, 0.01f, 0.5f) * 50.0f + 80.0f;
}

static void nilFunction(int id, sol::table block) {

}

void Server::init() {
    network.init();

    world.init(Context::SERVER);

    lua.init();
    lua.state["api"]["registerBlock"] = nilFunction;
    lua.runScripts();

    auto start = std::chrono::high_resolution_clock::now();

    sol::protected_function f(lua.state["api"]["initServer"]);
    sol::protected_function_result result = f();
    if (result.valid()) {

    }
    else {
        // Call failed
        sol::error err = result;
        std::cout << err.what() << std::endl;
    }

//    for (int x = 0; x < 16; x++) {
//        for (int z = 0; z < 16; z++) {
//            generateTerrain(world.getChunk(x, z));
//        }
//    }

    auto finish = std::chrono::high_resolution_clock::now();
    float seconds = std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count() / 1000.0f;

    std::cout << "completed in " << seconds << "s " << std::endl;

//    int range = 16;
//
//    sol::protected_function f(lua.state["api"]["generateChunk"]);
//
//
//
//    int i = 0;
//    int lastPercentage = 0;
//
//    auto start = std::chrono::high_resolution_clock::now();
//
//    for (int x = 0; x < range; x++) {
//        for (int z = 0; z < range; z++) {
//            int percentage = ((float)i / ((float)range * (float)range)) * 100.0f;
//            if (percentage != lastPercentage) {
//                std::cout << "(Server) Generating terrain: " << percentage << "%" << std::endl;
//                lastPercentage = percentage;
//            }
//
//
//            Chunk *chunk = world.getChunk(x, z);
//
//            generateTerrain(chunk);
//            sol::table t = f(x, z);
//            for (int i = 0; i < t.size(); i++) {
//                chunk->blocks[i] = t.get<int>(i);
//            }
//
////            sol::protected_function_result result = f(x, z);
////            if (result.valid()) {
////                sol::table t = result;
////
////
////            }
////            else {
////                // Call failed
////                sol::error err = result;
////                std::cout << err.what() << std::endl;
////            }
//            i++;
//        }
}

void Server::run(bool &running) {
    std::cout << "running server" << std::endl;

    init();

    while (running) {
        update();

    }

    std::cout << "Shutting down server" << std::endl;

}