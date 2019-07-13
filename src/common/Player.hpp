#pragma once

#include "Math.hpp"
#include "sol.hpp"

class Camera;

//forward declaration
class World;
class Client;

class Player {
public:
    vec3 position;
    vec3 velocity;
    vec3 direction = vec3(0.0f, 0.0f, -1.0f);
    bool onGround;
    int heldBlock = 4;

    float xRot = 0;
    float yRot = 0;

    World *world;
    Client &client;

    sol::table table;

    Player(World *world, Client &client);

    void update(Camera &cam, float delta);
};
