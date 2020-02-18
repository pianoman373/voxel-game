#pragma once

#include <thread>
#include <atomic>

class World;
class LuaHandler;

class WorldGenerator {
private:
    std::thread *thread0 = nullptr;

    std::atomic<bool> isFinished = false;

    void generateThread(int size);

    int size;

public:
    WorldGenerator(int size);

    void generate();

    bool isComplete();
};