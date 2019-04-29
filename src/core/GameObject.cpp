#include "GameObject.hpp"
#include "Renderer.hpp"
#include "RigidBody.hpp"

#include <iostream>

Component::~Component() {
    
}

void Component::setParent(GameObject *parent) {
    this->parent = parent;
}

GameObject *Component::getParent() const {
    return this->parent;
}

void Component::init() {

}

void Component::render() {

}

void Component::update(float) {

}

ModelComponent::ModelComponent(Mesh &mesh, Material &material): mesh(mesh), material(material) {

}

void ModelComponent::render() {
    Renderer::render(&mesh, &material, &this->getParent()->worldTransform);
}

Mesh& ModelComponent::getMesh() {
    return mesh;
}

Material& ModelComponent::getMaterial() {
    return material;
}

GameObject::~GameObject() {
    for (size_t i = 0; i < components.size(); i++) {
        delete components[i];
    }
    components.clear();

    if (this->rb) {
        delete this->rb;
    }
}

GameObject::GameObject(const Transform &transform, const std::string &name) {
    this->transform = transform;
    this->name = name;
    this->rb = nullptr;

}

GameObject& GameObject::createChild(const Transform &transform, const std::string &name) {
    GameObject *child = new GameObject(transform, name);
    child->parent = this;

    children.push_back(child);

	return *child;
}

int GameObject::getNumChildren() {
    return children.size();
}

GameObject& GameObject::getChild(int index) {
    return *children[index];
}

RigidBody *GameObject::addRigidBody(float mass, Scene &scene) {
    this->rb = new RigidBody(scene, this->transform, mass);

    return this->rb;
}

RigidBody *GameObject::getRigidBody() {
    return this->rb;
}

void GameObject::addComponent(Component *c) {
    this->components.push_back(c);
    c->setParent(this);

    c->init();
}

int GameObject::getNumComponents() {
    return components.size();
}

Component *GameObject::getComponent(int index) {
    return components.at(index);
}

void GameObject::render() {
    if (parent == nullptr) {
        worldTransform = transform;
    }
    else {
        worldTransform = parent->worldTransform * transform;
    }


    for (size_t i = 0; i < components.size(); i++) {
        components[i]->render();
    }

    for (size_t i = 0; i < children.size(); i++) {
        children[i]->render();
    }
}

void GameObject::update(float delta) {
    if (rb) {
        transform.position = rb->getPosition();
        transform.rotation = rb->getRotation();
    }


    for (size_t i = 0; i < components.size(); i++) {
        components[i]->update(delta);
    }
    for (size_t i = 0; i < children.size(); i++) {
        children[i]->update(delta);
    }
}

const std::string &GameObject::getName() const {
    return this->name;
}

void GameObject::setName(const std::string &name) {
    this->name = name;
}