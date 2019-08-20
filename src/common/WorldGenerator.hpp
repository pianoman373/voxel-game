#pragma once

class World;
class LuaHandler;

class WorldGenerator {
private:


public:
    void generate(LuaHandler &lua, int size);
};