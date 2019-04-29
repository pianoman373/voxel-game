#pragma once

#include "Model.hpp"
#include <vector>

class Scene;
class RigidBody;

class GameObject;

class Component {
private:
    GameObject *parent;

public:
    virtual ~Component();

    void setParent(GameObject *parent);

    GameObject *getParent() const;

    virtual void init();

    virtual void render();

    virtual void update(float delta);
};

class ModelComponent : public Component {
private:
    Mesh &mesh;
    Material &material;

public:
    ModelComponent(Mesh &mesh, Material &material);

    void render();

    Mesh &getMesh();

    Material &getMaterial();
};

class GameObject {
private:
    std::vector<Component*> components;
    std::string name;
    RigidBody *rb;

    GameObject *parent = nullptr;
    std::vector<GameObject*> children;

public:
    Transform transform;
    Transform worldTransform;

    ~GameObject();

    GameObject(const Transform &transform, const std::string &name);

    GameObject &createChild(const Transform &transform, const std::string &name);

    int getNumChildren();

    GameObject &getChild(int index);

    void render();

    void update(float delta);

    RigidBody *addRigidBody(float mass, Scene &scene);

    RigidBody *getRigidBody();

    void addComponent(Component *c);

    int getNumComponents();

    Component *getComponent(int index);

    const std::string &getName() const;

    void setName(const std::string &name);

    template <typename T>
    T *getComponent() {
        for (int i = 0; i < components.size(); i++) {
            T *ptr = dynamic_cast<T*>(components[i]);

            if (ptr) {
                return ptr;
            }
        }
        return nullptr;
    }

    template <typename T>
    std::vector<T*> getComponents() {
        std::vector<T*> ret;

        for (int i = 0; i < components.size(); i++) {
            T *ptr = dynamic_cast<T*>(components[i]);

            if (ptr) {
                ret.push_back(ptr);
            }
        }

        return ret;
    }
};

