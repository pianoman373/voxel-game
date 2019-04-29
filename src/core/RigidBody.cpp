#include "RigidBody.hpp"

RigidBody::RigidBody(Scene &scene, const Transform &transform, float mass) {
    // this->mass = mass;

    // //create a dynamic rigidbody
    // colShape = new btCompoundShape();

    // /// Create Dynamic Objects
    // btTransform startTransform;
    // startTransform.setIdentity();

    // startTransform.setOrigin(btVector3(transform.position.x, transform.position.y, transform.position.z));

    // //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
    // motionState = new btDefaultMotionState(startTransform);
    // btVector3 inertia(0,0,0);
    // btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, colShape, inertia);
    // body = new btRigidBody(rbInfo);

    // scene.getBulletWorld()->addRigidBody(body);
}

RigidBody::~RigidBody() {
    // for (int i = 0; i < colShape->getNumChildShapes(); i++) {
    //     delete colShape->getChildShape(i);
    // }

    // delete motionState;
    // delete colShape;
    // delete body;
}

vec3 RigidBody::getPosition() const {
    // btTransform trans;
    // body->getMotionState()->getWorldTransform(trans);

    // return vec3(float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
    return vec3();
}

quaternion RigidBody::getRotation() const {
    // btTransform trans;
    // body->getMotionState()->getWorldTransform(trans);

    // return quaternion(trans.getRotation().w(), trans.getRotation().x(), trans.getRotation().y(), trans.getRotation().z());
    return quaternion();
}

void RigidBody::setVelocity(const vec3 &velocity) {
    // body->activate(true);
    // body->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
}

vec3 RigidBody::getVelocity() const {
    // btVector3 v = body->getLinearVelocity();

    // return vec3(v.x(), v.y(), v.z());

    return vec3();
}

void RigidBody::setAngularFactor(float factor) {
    // body->setAngularFactor(btVector3(factor, factor, factor));
}

RigidBody *RigidBody::addBoxCollider(const vec3 &origin, const vec3 &halfExtents) {
    // btTransform t;  //position and rotation
    // t.setIdentity();
    // t.setOrigin(btVector3(origin.x,origin.y,origin.z));
    // colShape->addChildShape(t, new btBoxShape(btVector3(halfExtents.x,halfExtents.y,halfExtents.z)));

    // /*if (mass > 0.0f) {
    //     btVector3 inertia(0,0,0);
    //     btScalar masses[colShape->getNumChildShapes()] = { mass/colShape->getNumChildShapes()};
    //     colShape->calculatePrincipalAxisTransform(masses,t,inertia);

    //     body->setMassProps(mass, inertia);
    //     body->updateInertiaTensor();
    // }*/

    return this;
}

RigidBody *RigidBody::addSphereCollider(const vec3 &origin, float radius) {
    // btTransform t;  //position and rotation
    // t.setIdentity();
    // t.setOrigin(btVector3(origin.x,origin.y,origin.z));
    // colShape->addChildShape(t, new btSphereShape(radius));

    // /*if (mass > 0.0f) {
    //     btVector3 inertia(0,0,0);
    //     btScalar masses[colShape->getNumChildShapes()] = { mass/colShape->getNumChildShapes()};
    //     colShape->calculatePrincipalAxisTransform(masses,t,inertia);

    //     body->setMassProps(mass, inertia);
    //     body->updateInertiaTensor();
    // }*/


    return this;
}

RigidBody *RigidBody::addCylinderCollider(const vec3 &origin, float radius, float height) {
    // btTransform t;  //position and rotation
    // t.setIdentity();
    // t.setOrigin(btVector3(origin.x,origin.y,origin.z));
    // colShape->addChildShape(t, new btCylinderShape(btVector3(radius, height, radius)));

    // /*if (mass > 0.0f) {
    //     btVector3 inertia(0,0,0);
    //     btScalar masses[colShape->getNumChildShapes()] = { mass/colShape->getNumChildShapes()};
    //     colShape->calculatePrincipalAxisTransform(masses,t,inertia);

    //     body->setMassProps(mass, inertia);
    //     body->updateInertiaTensor();
    // }*/

    return this;
}

RigidBody *RigidBody::addMeshCollider(const vec3 &origin, const Mesh &mesh, const vec3 &scale) {
    // if (mesh.indices.size() > 0) {
    //     btTriangleMesh *tm = new btTriangleMesh();

    //     for (size_t i = 0; i < mesh.indices.size(); i += 3) {
    //         vec3 v1 = mesh.positions[mesh.indices[i]] * scale;
    //         vec3 v2 = mesh.positions[mesh.indices[i+1]] * scale;
    //         vec3 v3 = mesh.positions[mesh.indices[i+2]] * scale;

    //         tm->addTriangle(btVector3(v1.x, v1.y, v1.z), btVector3(v2.x, v2.y, v2.z), btVector3(v3.x, v3.y, v3.z));
    //     }

    //     btBvhTriangleMeshShape *ms = new btBvhTriangleMeshShape(tm, false);

    //     btTransform t;  //position and rotation
    //     t.setIdentity();
    //     t.setOrigin(btVector3(origin.x,origin.y,origin.z));
    //     colShape->addChildShape(t, ms);
    // }



    /*if (mass > 0.0f) {
        btVector3 inertia(0,0,0);
        btScalar masses[colShape->getNumChildShapes()] = { mass/colShape->getNumChildShapes()};
        colShape->calculatePrincipalAxisTransform(masses,t,inertia);

        body->setMassProps(mass, inertia);
        body->updateInertiaTensor();
    }*/

    return this;
}