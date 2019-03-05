#include "Player.hpp"
#include "Chunk.hpp"
#include "World.hpp"
#include "Client.hpp"
#include "Block.hpp"

#include <crucible/AABB.hpp>
#include <crucible/Renderer.hpp>
#include <crucible/Camera.hpp>
#include <crucible/Input.hpp>

#include <imgui.h>
#include <GLFW/glfw3.h>

static const float movementSpeed = 7.0f;
static const float mouseSensitivity = 8.0f;
static const float gravity = 22.8f;
static const float jumpPower = 7.7f;

static bool p_open = false;

Player::Player(World &world, Client &client): world(world), client(client) {

}

void Player::update(Camera &cam, float delta) {
    vec3 oldPosition = position;
    bool breakBlock = false;
    bool placeBlock = false;

    //mouse movement input
    static vec2 lastpos;

    if (Input::isMouseGrabbed()) {
        vec2 offset = Input::getCursorPos() - lastpos;
        float xOffset = -offset.x / mouseSensitivity;
        float yOffset = offset.y / mouseSensitivity;

        yRot += (yOffset);
        xRot += (xOffset);

        if (yRot > 89.9f)
            yRot = 89.9f;
        if (yRot < -89.9f)
            yRot = -89.9f;

        mat4 mat;
        mat = rotate(mat, vec3(0.0f, 1.0f, 0.0f), xRot);
        mat = rotate(mat, vec3(1.0f, 0.0f, 0.0f), yRot);

        vec4 vec = mat * vec4(vec3(0.0f, 0.0f, 1.0f), 1.0f);

        cam.setDirection(vec3(vec));

        lastpos = Input::getCursorPos();

        //get mouse input for breaking and placing
        static bool leftMouse = false;

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

        //apply movement
        if (Input::isKeyDown(GLFW_KEY_W)) {
            vec3 dir = cam.getDirection();
            dir.y = 0.0f;
            dir = normalize(dir);
            dir = dir * movementSpeed;
            velocity = velocity + dir;
        }
        if (Input::isKeyDown(GLFW_KEY_S)) {
            vec3 dir = cam.getDirection();
            dir.y = 0.0f;
            dir = normalize(dir);
            dir = dir * movementSpeed;
            velocity = velocity - dir;
        }
        if (Input::isKeyDown(GLFW_KEY_A)) {
            vec3 dir = cam.getRight();
            dir.y = 0.0f;
            dir = normalize(dir);
            dir = dir * movementSpeed;
            velocity = velocity - dir;
        }
        if (Input::isKeyDown(GLFW_KEY_D)) {
            vec3 dir = cam.getRight();
            dir.y = 0.0f;
            dir = normalize(dir);
            dir = dir * movementSpeed;
            velocity = velocity + dir;
        }
        if (Input::isKeyDown(GLFW_KEY_SPACE) && onGround) {
            velocity.y = jumpPower;
        }
        if (Input::isKeyDown(GLFW_KEY_R)) {
            velocity.y = 8.4f;
        }
        if (Input::isKeyDown(GLFW_KEY_F)) {
            velocity.y = -8.4f;
        }
    }

    //apply gravity
    velocity.y -= gravity * delta;

    vec3 velocityDistance = (velocity * delta);
    //position = position + velocityDistance;

    //the actual player collider (set to null for now)
    AABB playerBoundingBox = AABB(vec3(), vec3());

    //the collider for narrowing down all the world's blocks to a smaller area
    AABB playerBoundingBoxLarge = AABB(position - vec3(5.0f), position + vec3(5.0f));

    //Renderer::renderDebugAABB(playerBoundingBoxLarge, vec3(1.0f, 0.0f, 0.0f));

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
            if (velocityDistance.y < 0.0f && playerBoundingBox.min.y > blockAbb.max.y) {

                //distance between the nearest cube sides               V bias needed
                float dist = playerBoundingBox.min.y - (blockAbb.max.y + 0.0001f);

                //if distance to block is closer than distance about to be traveled, truncate that distance to meet the block
                if (dist < -velocityDistance.y) {
                    velocityDistance.y = -dist;
                    onGround = true;
                    velocity.y = 0;
                }
            }
            //for moving +y
            if (velocityDistance.y > 0.0f && playerBoundingBox.max.y < blockAbb.min.y) {

                //distance between the nearest cube sides
                //                             V bias needed
                float dist = (blockAbb.min.y - 0.0001f) - playerBoundingBox.max.y;

                //if distance to block is closer than distance about to be traveled, truncate that distance to meet the block
                if (dist < velocityDistance.y) {
                    velocityDistance.y = dist;
                    velocity.y = 0;
                }
            }
        }
    }

    position.y += velocityDistance.y;

    //X axis collision
    playerBoundingBox = AABB(position - vec3(0.3f, 0.9f, 0.3f), position + vec3(0.3f, 0.9f, 0.3f));
    for (AABB &blockAbb: collisions) {
        //test if block intersects on the 2D YZ axis
        if (blockAbb.max.y > playerBoundingBox.min.y && blockAbb.min.y < playerBoundingBox.max.y && blockAbb.max.z > playerBoundingBox.min.z && blockAbb.min.z < playerBoundingBox.max.z)
        {
            //for moving -x
            if (velocityDistance.x < 0.0f && playerBoundingBox.min.x > blockAbb.max.x) {

                //distance between the nearest cube sides               V bias needed
                float dist = playerBoundingBox.min.x - (blockAbb.max.x + 0.0001f);

                //if distance to block is closer than distance about to be traveled, truncate that distance to meet the block
                if (dist < -velocityDistance.x) {
                    velocityDistance.x = -dist;
                }
            }
            //for moving +x
            if (velocityDistance.x > 0.0f && playerBoundingBox.max.x < blockAbb.min.x) {

                //distance between the nearest cube sides
                //                             V bias needed
                float dist = (blockAbb.min.x - 0.0001f) - playerBoundingBox.max.x;

                //if distance to block is closer than distance about to be traveled, truncate that distance to meet the block
                if (dist < velocityDistance.x) {
                    velocityDistance.x = dist;
                }
            }
        }
    }
    position.x += velocityDistance.x;


    //Z axis collision
    playerBoundingBox = AABB(position - vec3(0.3f, 0.9f, 0.3f), position + vec3(0.3f, 0.9f, 0.3f));
    for (AABB &blockAbb: collisions) {
        //test if block intersects on the 2D XY axis
        if (blockAbb.max.x > playerBoundingBox.min.x && blockAbb.min.x < playerBoundingBox.max.x && blockAbb.max.y > playerBoundingBox.min.y && blockAbb.min.y < playerBoundingBox.max.y)
        {
            //for moving -z
            if (velocityDistance.z < 0.0f && playerBoundingBox.min.z > blockAbb.max.z) {

                //distance between the nearest cube sides               V bias needed
                float dist = playerBoundingBox.min.z - (blockAbb.max.z + 0.0001f);

                //if distance to block is closer than distance about to be traveled, truncate that distance to meet the block
                if (dist < -velocityDistance.z) {
                    velocityDistance.z = -dist;
                }
            }
            //for moving +z
            if (velocityDistance.z > 0.0f && playerBoundingBox.max.z < blockAbb.min.z) {

                //distance between the nearest cube sides
                //                             V bias needed
                float dist = (blockAbb.min.z - 0.0001f) - playerBoundingBox.max.z;

                //if distance to block is closer than distance about to be traveled, truncate that distance to meet the block
                if (dist < velocityDistance.z) {
                    velocityDistance.z = dist;
                }
            }
        }
    }
    position.z += velocityDistance.z;

    cam.setPosition(position + vec3(0.0f, 0.7f, 0.0f));

    //block placing
    vec3i blockpos;
    vec3 blocknormal;
    if (world.raycastBlocks(cam.getPosition(), cam.getDirection(), 10.0f, blockpos, blocknormal)) {
        float bias = 0.01f;
        //block outline for the block the player is looking at
        Renderer::debug.renderDebugAABB(vec3(blockpos.x, blockpos.y, blockpos.z) - bias, vec3(blockpos.x + 1, blockpos.y + 1, blockpos.z + 1) + bias, vec3());

		if (breakBlock) {
			int x = blockpos.x;
			int y = blockpos.y;
			int z = blockpos.z;



            world.setBlock(x, y, z, 0);

            Packet p;
            uint16_t packetID = 4;
            int blockID = 0;

            p << packetID << x << y << z << blockID;

            client.network.sendPacket(p);
		}

        if (placeBlock) {
            //if (!playerBoundingBox.intersectsWith(AABB(vec3(blockpos.x + blocknormal.x, blockpos.y + blocknormal.y, blockpos.z + blocknormal.z), vec3(blockpos.x  + blocknormal.x + 1, blockpos.y  + blocknormal.y + 1, blockpos.z  + blocknormal.z + 1)))) {
            int x = blockpos.x + blocknormal.x;
            int y = blockpos.y + blocknormal.y;
            int z = blockpos.z + blocknormal.z;
            int blockID = client.lua.state.get<int>("hotbarSelectorPos")+1;

            world.setBlock(x, y, z, blockID);

            Packet p;
            uint16_t packetID = 4;

            p << packetID << x << y << z << blockID;

            client.network.sendPacket(p);
        }
    }

    int scroll = (int)Input::getScroll();
    heldBlock += scroll;

    velocity.x = 0.0f;
    //velocity.y = 0.0f;
    velocity.z = 0.0f;

    ///vec3i chunkPos = World::worldToChunkPos(position);

    vec2i chunkPos = vec2i((int)position.x >> 4, (int)position.z >> 4);

    // freezes player if not in chunk
    if (!world.chunkExists(chunkPos.x, chunkPos.y)) {
        position = oldPosition;
        velocity.y = 0.0f;
    }
}
