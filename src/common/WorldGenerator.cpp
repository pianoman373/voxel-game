#include "common/WorldGenerator.hpp"
#include "common/World.hpp"
#include "common/LuaHandler.hpp"
#include "server/ChunkIO.hpp"

#include <iostream>
#include <fstream>
#include <chrono>


#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>


void WorldGenerator::generate(int size) {
    LuaHandler lua;
    World world(lua);

    
    lua.init();
    lua.addCommonFunctions(world);
    lua.runScripts();

    try {
        lua.state.safe_script_file("mods/base/worldgen.lua");
    }
    catch( const sol::error& e ) {
        std::cout << e.what() << std::endl;
    }

    

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


    // std::cout << "generating heightmaps..." << std::endl;
    // start = std::chrono::high_resolution_clock::now();

    // for (int x = 0; x < size; x++) {
    //     for (int z = 0; z < size; z++) {
    //         std::cout << (float)((x * size) + z) / (float)(size*size) << std::endl; 
    //         world.getChunk(x, z)->calculateHeightmap();
    //     }
    // }

    // std::cout << "Generating map" << std::endl;

    // unsigned char *data = new unsigned char[size*16*size*16*3];

    // for (int x = 0; x < size; x++) {
    //     for (int z = 0; z < size; z++) {
    //         Chunk *c = world.getChunk(x, z).get();

    //         for (int cx = 0; cx < 16; cx++) {
    //             for (int cz = 0; cz < 16; cz++) {
    //                 unsigned char height = c->heightMap[cx][cz];

    //                 int worldX = (x*16)+cx;
    //                 int worldZ = (z*16)+cz;

    //                 long index = ((worldZ * size*16*3) + worldX*3);

    //                 Block &b = c->getBlock(cx, 70, cz);
    //                 uint32_t color = b.color;



    //                 data[index] = (color >> 16) & 0xFF;
    //                 data[index+1] =(color >> 8) & 0xFF;
    //                 data[index+2] = color & 0xFF;
    //             }
    //         }
    //     }
    // }

    // stbi_write_png("save/map.png", size*16, size*16, 3, data, size*16 * 3);

    // finish = std::chrono::high_resolution_clock::now();
    // seconds = std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count() / 1000.0f;
    // std::cout << "completed in " << seconds << "s " << std::endl;


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