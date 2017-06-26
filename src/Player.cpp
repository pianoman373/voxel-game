#include "Player.hpp"
#include "Input.hpp"
#include "Chunk.hpp"
#include "AABB.hpp"
#include "Renderer.hpp"
#include "World.hpp"

static const float movementSpeed = 6.0f;

//TODO: maybe this should be a pointer rather than a reference
Player::Player(World &world): world(world) {

}

void Player::update(Camera &cam, float delta) {
    //get mouse input for breaking and placing
    static bool leftMouse = false;
    bool breakBlock = false;

    if (!leftMouse && Input::isMouseButtonDown(0)) {
        breakBlock = true;
        leftMouse = true;
    }
    else if (leftMouse && Input::isMouseButtonDown(0)) {
        breakBlock = false;
    }
    else if (leftMouse && !Input::isMouseButtonDown(0)) {
        leftMouse = false;
    }

    static bool rightMouse = false;
    bool placeBlock = false;

    if (!rightMouse && Input::isMouseButtonDown(1)) {
        placeBlock = true;
        rightMouse = true;
    }
    else if (rightMouse && Input::isMouseButtonDown(1)) {
        placeBlock = false;
    }
    else if (rightMouse && !Input::isMouseButtonDown(1)) {
        rightMouse = false;
    }

    //apply gravity
    velocity.y -= 0.45f * delta;

    //apply movement
    if (Input::isKeyDown(GLFW_KEY_W)) {
        vec3 dir = cam.getDirection();
        dir.y = 0.0f;
        dir = normalize(dir);
        dir = dir * movementSpeed * delta;
        velocity = velocity + dir;
    }
    if (Input::isKeyDown(GLFW_KEY_S)) {
        vec3 dir = cam.getDirection();
        dir.y = 0.0f;
        dir = normalize(dir);
        dir = dir * movementSpeed * delta;
        velocity = velocity - dir;
    }
    if (Input::isKeyDown(GLFW_KEY_A)) {
        vec3 dir = cam.getRight();
        dir.y = 0.0f;
        dir = normalize(dir);
        dir = dir * movementSpeed * delta;
        velocity = velocity - dir;
    }
    if (Input::isKeyDown(GLFW_KEY_D)) {
        vec3 dir = cam.getRight();
        dir.y = 0.0f;
        dir = normalize(dir);
        dir = dir * movementSpeed * delta;
        velocity = velocity + dir;
    }
    if (Input::isKeyDown(GLFW_KEY_SPACE) && onGround) {
        velocity.y = 0.14f;
    }

    //the actual player collider (set to null for now)
    AABB playerBoundingBox = AABB(vec3(), vec3());

    //the collider for narrowing down all the world's blocks to a smaller area
    AABB playerBoundingBoxLarge = AABB(position - vec3(5.0f), position + vec3(5.0f));

    std::vector<AABB> collisions = world.getCollisions(playerBoundingBoxLarge);

    //collision detection

    onGround = false;

    //Y axis collision
    playerBoundingBox = AABB(position - vec3(0.3f, 0.9f, 0.3f), position + vec3(0.3f, 0.9f, 0.3f));
    for (AABB &blockAbb: collisions) {
        //test if block intersects on the 2D XZ axis
        if (blockAbb.max.x > playerBoundingBox.min.x && blockAbb.min.x < playerBoundingBox.max.x && blockAbb.max.z > playerBoundingBox.min.z && blockAbb.min.z < playerBoundingBox.max.z)
        {
            //for moving -y
            if (velocity.y < 0.0f && playerBoundingBox.min.y > blockAbb.max.y) {

                //distance between the nearest cube sides               V bias needed
                float dist = playerBoundingBox.min.y - (blockAbb.max.y + 0.0001f);

                //if distance to block is closer than distance about to be traveled, truncate that distance to meet the block
                if (dist < -velocity.y) {
                    velocity.y = -dist;
                    onGround = true;
                }
            }
            //for moving +y
            if (velocity.y > 0.0f && playerBoundingBox.max.y < blockAbb.min.y) {

                //distance between the nearest cube sides
                //                             V bias needed
                float dist = (blockAbb.min.y - 0.0001f) - playerBoundingBox.max.y;

                //if distance to block is closer than distance about to be traveled, truncate that distance to meet the block
                if (dist < velocity.y) {
                    velocity.y = dist;
                }
            }
        }
    }

    position.y += velocity.y;

    //X axis collision
    playerBoundingBox = AABB(position - vec3(0.3f, 0.9f, 0.3f), position + vec3(0.3f, 0.9f, 0.3f));
    for (AABB &blockAbb: collisions) {
        //test if block intersects on the 2D YZ axis
        if (blockAbb.max.y > playerBoundingBox.min.y && blockAbb.min.y < playerBoundingBox.max.y && blockAbb.max.z > playerBoundingBox.min.z && blockAbb.min.z < playerBoundingBox.max.z)
        {
            //for moving -x
            if (velocity.x < 0.0f && playerBoundingBox.min.x > blockAbb.max.x) {

                //distance between the nearest cube sides               V bias needed
                float dist = playerBoundingBox.min.x - (blockAbb.max.x + 0.0001f);

                //if distance to block is closer than distance about to be traveled, truncate that distance to meet the block
                if (dist < -velocity.x) {
                    velocity.x = -dist;
                }
            }
            //for moving +x
            if (velocity.x > 0.0f && playerBoundingBox.max.x < blockAbb.min.x) {

                //distance between the nearest cube sides
                //                             V bias needed
                float dist = (blockAbb.min.x - 0.0001f) - playerBoundingBox.max.x;

                //if distance to block is closer than distance about to be traveled, truncate that distance to meet the block
                if (dist < velocity.x) {
                    velocity.x = dist;
                }
            }
        }
    }
    position.x += velocity.x;


    //Z axis collision
    playerBoundingBox = AABB(position - vec3(0.3f, 0.9f, 0.3f), position + vec3(0.3f, 0.9f, 0.3f));
    for (AABB &blockAbb: collisions) {
        //test if block intersects on the 2D XY axis
        if (blockAbb.max.x > playerBoundingBox.min.x && blockAbb.min.x < playerBoundingBox.max.x && blockAbb.max.y > playerBoundingBox.min.y && blockAbb.min.y < playerBoundingBox.max.y)
        {
            //for moving -z
            if (velocity.z < 0.0f && playerBoundingBox.min.z > blockAbb.max.z) {

                //distance between the nearest cube sides               V bias needed
                float dist = playerBoundingBox.min.z - (blockAbb.max.z + 0.0001f);

                //if distance to block is closer than distance about to be traveled, truncate that distance to meet the block
                if (dist < -velocity.z) {
                    velocity.z = -dist;
                }
            }
            //for moving +z
            if (velocity.z > 0.0f && playerBoundingBox.max.z < blockAbb.min.z) {

                //distance between the nearest cube sides
                //                             V bias needed
                float dist = (blockAbb.min.z - 0.0001f) - playerBoundingBox.max.z;

                //if distance to block is closer than distance about to be traveled, truncate that distance to meet the block
                if (dist < velocity.z) {
                    velocity.z = dist;
                }
            }
        }
    }
    position.z += velocity.z;

    cam.setPosition(position + vec3(0.0f, 0.8f, 0.0f));

    //block placing
    vec3i blockpos;
    vec3 blocknormal;
    if (world.raycastBlocks(cam.getPosition(), cam.getDirection(), 10.0f, blockpos, blocknormal)) {
        float bias = 0.01f;
        //block outline for the block the player is looking at
        Renderer::renderDebugAABB(vec3(blockpos.x, blockpos.y, blockpos.z) - bias, vec3(blockpos.x + 1, blockpos.y + 1, blockpos.z + 1) + bias, vec3());

        if (breakBlock) {
            world.setBlock(blockpos.x, blockpos.y, blockpos.z, 0);
        }
        if (placeBlock) {
            world.setBlock(blockpos.x + blocknormal.x, blockpos.y + blocknormal.y, blockpos.z + blocknormal.z, 1);
        }
    }

    velocity.x = 0.0f;
    velocity.z = 0.0f;
}
