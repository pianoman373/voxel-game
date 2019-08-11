#pragma once

#include "common/Entity.hpp"

#include "rendering/Material.hpp"
#include "rendering/Mesh.hpp"
#include "util/AABB.hpp"

class WorldRenderer;

class EntityRenderer {
private:
    Mesh mesh;
    Material material;
    Texture tex;

    WorldRenderer &wr;

public:
    EntityRenderer(WorldRenderer &wr);

    void init();

    void render(Entity &e);
};