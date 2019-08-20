#include "common/WorldGenerator.hpp"
#include "common/World.hpp"
#include "common/LuaHandler.hpp"
#include "server/ChunkIO.hpp"

#include <iostream>
#include <fstream>
#include <chrono>


void WorldGenerator::generate(LuaHandler &lua, int size) {
    try {
        lua.state.safe_script_file("mods/base/worldgen.lua");
    }
    catch( const sol::error& e ) {
        std::cout << e.what() << std::endl;
    }

    World world(lua);

    auto start = std::chrono::high_resolution_clock::now();

    std::cout << "generating terrain..." << std::endl;

    for (int x = 0; x < size; x++) {
        for (int z = 0; z < size; z++) {
            
            sol::protected_function f(lua.state["generateChunk"]);
            sol::protected_function_result result = f(x, z, world.getChunk(x, z).get());
            if (result.valid()) {

            }
            else {
                // Call failed
                sol::error err = result;
                std::cout << err.what() << std::endl;
            }
        }
    }

    std::cout << "decorating terrain..." << std::endl;

    for (int x = 0; x < size; x++) {
        for (int z = 0; z < size; z++) {
            sol::protected_function f(lua.state["decorateChunk"]);
            sol::protected_function_result result = f(x, z, world);
            if (result.valid()) {

            }
            else {
                // Call failed
                sol::error err = result;
                std::cout << err.what() << std::endl;
            }
        }
    }

    auto finish = std::chrono::high_resolution_clock::now();
    float seconds = std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count() / 1000.0f;
    std::cout << "completed in " << seconds << "s " << std::endl;


    std::cout << "generating heightmaps..." << std::endl;
    start = std::chrono::high_resolution_clock::now();

    //generate lighting
    for (int x = 0; x < size; x++) {
        for (int z = 0; z < size; z++) {
            world.getChunk(x, z)->calculateHeightmap();
        }
    }

    finish = std::chrono::high_resolution_clock::now();
    seconds = std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count() / 1000.0f;
    std::cout << "completed in " << seconds << "s " << std::endl;


    std::cout << "generating lighting..." << std::endl;
    start = std::chrono::high_resolution_clock::now();

    //generate lighting
    for (int x = 0; x < size; x++) {
        for (int z = 0; z < size; z++) {
            world.getChunk(x, z)->calculateSunLighting();
        }
    }

    finish = std::chrono::high_resolution_clock::now();
    seconds = std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count() / 1000.0f;
    std::cout << "completed in " << seconds << "s " << std::endl;


    ChunkIO chunkIO;

    for (int x = 0; x < size; x++) {
        for (int z = 0; z < size; z++) {
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