#include "common/Entity.hpp"
#include "common/LuaHandler.hpp"
#include "util/AABB.hpp"
#include "common/World.hpp"

Entity::Entity(World &world, LuaHandler &lua, sol::table baseTable, vec3 position): world(world) {
    table = lua.state.create_table();

    x = position.x;
    y = position.y;
    z = position.z;

    table["entity"] = this;

    std::cout << baseTable.get<std::string>("name") << std::endl;

    table[sol::metatable_key] = baseTable[sol::metatable_key];

    table["new"](table);
}

void Entity::tick() {
    //table["tick"](table);

    LuaHandler::safeCall(table.get<sol::safe_function>("tick"), table);

    vec3 position = getPosition();

    AABB playerBoundingBoxLarge = AABB(position - vec3(5.0f), position + vec3(5.0f));
    std::vector<AABB> collisions = world.getCollisions(playerBoundingBoxLarge);

    float gravity = 9.8f * 0.005f;


    //apply gravity
    yVelocity -= gravity;

    vec3 velocityDistance = (vec3(xVelocity, yVelocity, zVelocity));

    

    bool onGround = false;

    //Y axis collision
    AABB playerBoundingBox = AABB(position - vec3(0.3f, 0.9f, 0.3f), position + vec3(0.3f, 0.9f, 0.3f));
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
                    yVelocity = 0;
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
                    yVelocity = 0;
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


    x = position.x;
    y = position.y;
    z = position.z;
}

vec3 Entity::getPosition() {
    return vec3(x, y, z);
}


void EntityRegistry::registerEntity(LuaHandler &lua, const std::string &id, sol::table &table) {
    sol::table metatable = lua.state.create_table();
    metatable["__index"] = table;
    table[sol::metatable_key] = metatable;

    entities[id] = table;
}

sol::table EntityRegistry::getEntityTable(const std::string &id) {
    if (entities.count(id)) {
        std::cout << "found entity " << id << std::endl;
        
        return entities[id];
    }
    else {
        return sol::table();
    }
    
}
