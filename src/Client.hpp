#pragma once

#include <crucible/Camera.hpp>
#include <crucible/Model.hpp>
#include <crucible/Material.hpp>
#include <crucible/Bone.hpp>

#include "NetworkManagerClient.hpp"
#include "World.hpp"
#include "WorldRenderer.hpp"
#include "LuaHandler.hpp"
#include "Settings.hpp"
#include "TextureRegistry.hpp"
#include "ItemRenderer.hpp"

#include <crucible/Frustum.hpp>


#include <map>

class Client {
private:
    Camera camera;

    std::map<int, vec3> playerPositions;

    std::unordered_map<vec2i, int> expectedChunks;



    uint8_t *rleCache;

public:
    NetworkManagerClient network;
    World world;
    WorldRenderer worldRenderer;
    Player player;
    Frustum frustum;
    Settings settings;
    TextureRegistry textureRegistry;
    LuaHandler lua;
    ItemRenderer itemRenderer;

    Model playerModel;
    Texture playerTexture;
    Transform playerTransform;
    Material playerMaterial;

    Bone root;

    Client();

    ~Client();

    void manageChunks(vec2i playerChunkPosition);

    void requestChunkFromServer(int x, int z);

    void receivePackets();

    void init(std::string address, int port);

    void update(float delta);

    void render();

    void run(bool &running, std::string address, int port);
};