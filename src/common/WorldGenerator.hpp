#pragma once

#include <thread>
#include <atomic>
#include <string>

class World;
class LuaHandler;

class WorldGenerator {
private:
    std::thread *thread0 = nullptr;

    std::atomic<bool> isFinished = false;

    void generateThread(std::string folder, int size);

    int size;
    std::string folder;

public:
    WorldGenerator(std::string folder, int size);

    void generate();

    bool isComplete();
};