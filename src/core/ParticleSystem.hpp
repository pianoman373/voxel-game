#pragma once

#include "Mesh.hpp"
#include "Material.hpp"
#include "Camera.hpp"

#include <functional>

struct ParticleInfo {
    vec3 position = vec3(0.0f);
    vec3 velocity = vec3(0.0f);
    vec3 color = vec3(1.0f);
    float age = 0.0f;
    float size = 0.0f;
};

class ParticleSystem {
public:
    Mesh particleMesh;
    Material particleMaterial;

    bool despawn = false;
    bool sorting = true;

    int particleCount = 3000;
    float lifetime = 4.0f;
    float particleSize = 0.2f;
    vec3 startingVelocity = vec3(0.0f, 20.0f, 0.0f);
    vec3 constantForce = vec3(0.0f, -9.8f, 0.0f);

    vec3 velocityVariation = vec3(3.0f, 0.0f, 3.0f);

    std::vector<ParticleInfo> particles;

    Transform transform;

    std::function<void(ParticleInfo &, float)> updateCallback;
    std::function<ParticleInfo(void)> spawnCallback;


    ParticleSystem();


    void init();

    void render();

    ParticleInfo spawnParticle();

    void updateParicle(ParticleInfo &p, float delta);

    void update(const Camera &cam);

    void setTexture(Texture &tex);
};