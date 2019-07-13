#pragma once

#include "Math.hpp"

#include <map>
#include "sol.hpp"

class LuaHandler;
class World;

class Entity {
private:
    

public:
    sol::table table;

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    float xVelocity = 0.0f;
    float yVelocity = 0.0f;
    float zVelocity = 0.0f;

    float width = 0.0f;
    float height = 0.0f;
    float depth = 0.0f;

    World &world;

    Entity(World &world, LuaHandler &lua, sol::table baseTable, vec3 position);

    void tick();

    vec3 getPosition();
};

class EntityRegistry {
private:
    std::map<std::string, sol::table> entities;

public:
    void registerEntity(LuaHandler &lua, const std::string &id, sol::table &table);

    sol::table getEntityTable(const std::string &id);
};