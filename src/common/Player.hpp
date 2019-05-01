#pragma once

#include "Math.hpp"

class Camera;

//forward declaration
class World;
class Client;

class Player {
public:
    vec3 position;
    vec3 velocity;
    bool onGround;
    int heldBlock = 4;

    float xRot = 0;
    float yRot = 0;

    World &world;
    Client &client;

    Player(World &world, Client &client);

    void update(Camera &cam, float delta);
};
