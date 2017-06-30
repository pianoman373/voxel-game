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

    float xRot = 0;
    float yRot = 0;

    World &world;

    Player(World &world);

    void update(Camera &cam, float delta);
};
