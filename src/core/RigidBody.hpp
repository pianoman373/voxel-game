#pragma once

#include "Scene.hpp"

#include <vector>

class btDefaultMotionState;
class btCompoundShape;
class btRigidBody;

class RigidBody {
private:
    btDefaultMotionState* motionState;
    btCompoundShape* colShape;

    btRigidBody* body;
    float mass;

public:
    RigidBody(Scene &scene, const Transform &transform, float mass);

    ~RigidBody();

    vec3 getPosition() const;

    quaternion getRotation() const;

    void setVelocity(const vec3 &velocity);

    vec3 getVelocity() const;

    void setAngularFactor(float factor);

    RigidBody *addBoxCollider(const vec3 &origin, const vec3 &halfExtents);

    RigidBody *addSphereCollider(const vec3 &origin, float radius);

    RigidBody *addCylinderCollider(const vec3 &origin, float radius, float height);

    RigidBody *addMeshCollider(const vec3 &origin, const Mesh &mesh, const vec3 &scale);
};