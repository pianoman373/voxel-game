#include "ParticleSystem.hpp"

#include "Resources.hpp"
#include "Renderer.hpp"
#include "Window.hpp"

#include <glad/glad.h>

ParticleInfo ParticleSystem::spawnParticle() {
    ParticleInfo info;
    info.position = vec3(0.0f, 0.0f, 0.0f);
    info.velocity = startingVelocity + vec3(srandf()*velocityVariation.x, srandf()*velocityVariation.y, srandf()*velocityVariation.z);
    info.age = 0.0f;
    info.size = 0.0f;

    return info;
}

void ParticleSystem::updateParicle(ParticleInfo &p, float delta) {
    p.velocity = p.velocity + (constantForce * delta);
    p.position = p.position + p.velocity * delta;
    

    float easeIn = p.age*10.0f/lifetime;
    if (easeIn > 1.0f) {
        easeIn = 1.0f;
    }

    p.size = easeIn*((lifetime-p.age)/lifetime)*particleSize;   
}

ParticleSystem::ParticleSystem() {
    updateCallback = [&] (ParticleInfo &p, float delta) {
        updateParicle(p, delta);
    };
    spawnCallback = [&] () {
        return spawnParticle();
    };
}

void ParticleSystem::init() {
    
    
    for (int i = 0; i < particleCount; i++) {
        ParticleInfo info = spawnCallback();
        info.age = -(i / static_cast <float> (particleCount))*lifetime;

        particles.push_back(info);
    }

    particleMesh.positions.resize(particles.size(), vec3(0.0f));
    particleMesh.normals.resize(particles.size(), vec3(1.0f));
    particleMesh.uvs.resize(particles.size(), vec2(0.0f, 0.0f));

    particleMesh.renderMode = GL_POINTS;
    particleMesh.generate();

    

    particleMaterial.deferred = false;
    particleMaterial.setShader(Resources::particleShader);
}

void ParticleSystem::render() {
    Renderer::render(&particleMesh, &particleMaterial, &transform);
}

void ParticleSystem::update(const Camera &cam) {
    static float lastTime = Window::getTime();
    static float delta = 0.0f;
    delta = Window::getTime() - lastTime;
    lastTime = Window::getTime();

    if (sorting) {
        std::sort(particles.begin(), particles.end(), [&](ParticleInfo &a, ParticleInfo &b) {
            vec4 apos = cam.getView() * vec4(a.position, 1.0f);
            vec4 bpos = cam.getView() * vec4(b.position, 1.0f);

            return apos.z < bpos.z;
        });
    }
    

    for (int i = 0; i < particles.size(); i++) {
        ParticleInfo &p = particles[i];

        if (despawn) {
            if (p.age > lifetime) {
                float newAge = p.age - lifetime;
                p = spawnCallback();
                p.age = newAge;
            }
        }
        p.age += delta;
        
        if (p.age >= 0) {
            updateCallback(p, delta);

            particleMesh.positions[i] = p.position;
            particleMesh.normals[i] = p.color;
            particleMesh.uvs[i].x = p.size;   
        }
          
    }
    particleMesh.generate();
}

void ParticleSystem::setTexture(Texture &tex) {
    particleMaterial.setUniformTexture("texture0", tex, 0);
}