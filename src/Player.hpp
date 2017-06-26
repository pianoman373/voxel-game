#pragma once

#include "Math.hpp"
#include "Camera.hpp"

//forward declaration
class World;

class Player {
public:
    vec3 position;
    vec3 velocity;
    bool onGround;

    World &world;

    Player(World &world);

    void update(Camera &cam, float delta);
};