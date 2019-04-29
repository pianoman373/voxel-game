#pragma once

#include "GameObject.hpp"
#include <vector>

class CrucibleBulletDebugDraw;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;

class Scene {
private:
    std::vector<GameObject*> objects;

    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* overlappingPairCache;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* dynamicsWorld;
    CrucibleBulletDebugDraw* debugDrawer;

    bool physicsEnabled = false;

public:
    ~Scene();

    GameObject &createObject(const Transform &transform, const std::string &name);

    GameObject &createMeshObject(Mesh &mesh, Material &material, const Transform &transform, const std::string &name);

    void render();

    void update(float delta);

    void setupPhysicsWorld();

    btDiscreteDynamicsWorld *getBulletWorld();

    bool isPhysicsEnabled() const;

    int numObjects();

    GameObject &getObject(int index);
};