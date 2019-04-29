#include "Scene.hpp"
#include "Renderer.hpp"

Scene::~Scene() {
    for (size_t i = 0; i < objects.size(); i++) {
        delete objects[i];
    }
    objects.clear();

    delete debugDrawer;
}

GameObject &Scene::createObject(const Transform &transform, const std::string &name) {
    GameObject *obj = new GameObject(transform, name);
    objects.push_back(obj);

    return *obj;
}

GameObject &Scene::createMeshObject(Mesh &mesh, Material &material, const Transform &transform, const std::string &name) {
    GameObject &obj = createObject(transform, name);
    obj.addComponent(new ModelComponent(mesh, material));

    return obj;
}

void Scene::render() {
    if (physicsEnabled) {
        //dynamicsWorld->debugDrawWorld();
    }

    for (size_t i = 0; i < objects.size(); i++) {
        objects[i]->render();
    }
}

void Scene::update(float delta) {
    //dynamicsWorld->stepSimulation(delta, 10);

    for (size_t i = 0; i < objects.size(); i++) {
        objects[i]->update(delta);
    }
}

void Scene::setupPhysicsWorld() {
    // ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
    // collisionConfiguration = new btDefaultCollisionConfiguration();

    // ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    // dispatcher = new btCollisionDispatcher(collisionConfiguration);

    // ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
    // overlappingPairCache = new btDbvtBroadphase();

    // ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    // solver = new btSequentialImpulseConstraintSolver;

    // dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

    // debugDrawer = new CrucibleBulletDebugDraw();
    // dynamicsWorld->setDebugDrawer(debugDrawer);
    // dynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe);

    // dynamicsWorld->setGravity(btVector3(0, -9.8f, 0));

    // physicsEnabled = true;
}

btDiscreteDynamicsWorld *Scene::getBulletWorld() {
    return dynamicsWorld;
}

bool Scene::isPhysicsEnabled() const {
    return physicsEnabled;
}

int Scene::numObjects() {
    return objects.size();
}

GameObject &Scene::getObject(int index) {
    return *objects[index];
}