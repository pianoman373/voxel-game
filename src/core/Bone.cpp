#include "Bone.hpp"
#include "Renderer.hpp"

Bone::Bone() {

}

Bone::Bone(const std::string &name, vec3 position, quaternion rotation) {
    this->name = name;
    this->position = position;
    this->rotation = rotation;
    this->startingPosition = position;
    this->startingRotation = rotation;
}

Bone &Bone::addChild(Bone b) {
    children.push_back(b);

    return children.back();
}

Bone *Bone::find(const std::string &boneName) {
    if (this->name == boneName) {
        return this;
    }

    for (size_t i = 0; i < children.size(); i++) {
        Bone *b = children[i].find(boneName);

        if (b) {
            return b;
        }
    }

    return nullptr;
}

mat4 Bone::getLocalTransform() const {
    mat4 mat;
    mat = translate(mat, position);
    mat = mat * toMatrix(rotation);

    return mat;
};

mat4 Bone::getStartingTransform() const {
    mat4 mat;
    mat = translate(mat, startingPosition);
    mat = mat * toMatrix(startingRotation);

    return mat;
};

std::vector<mat4> Bone::getStartingTransforms() const {
    std::vector<mat4> transforms;

    mat4 local = getStartingTransform();

    transforms.push_back(local);

    for (size_t i = 0; i < children.size(); i++) {
        std::vector<mat4> childTransforms = children[i].getStartingTransforms();

        for (size_t j = 0; j < childTransforms.size(); j++) {
            transforms.push_back(local * childTransforms[j]);
        }
    }

    return transforms;
}


std::vector<mat4> Bone::getSkeletonTransforms() const {
    std::vector<mat4> transforms;

    mat4 local = getLocalTransform();

    transforms.push_back(local);

    for (size_t i = 0; i < children.size(); i++) {
        std::vector<mat4> childTransforms = children[i].getSkeletonTransforms();

        for (size_t j = 0; j < childTransforms.size(); j++) {
            transforms.push_back(local * childTransforms[j]);
        }
    }

    return transforms;
}

std::vector<mat4> Bone::getSkinningTransforms() const {
    std::vector<mat4> transforms = getSkeletonTransforms();
    std::vector<mat4> startingTransforms = getStartingTransforms();

    for (size_t i = 0; i < transforms.size(); i++) {
        transforms[i] =  transforms[i] * inverse(startingTransforms[i]);
    }

    return transforms;
}

static void renderMat4(mat4 mat) {
    Renderer::debug.renderDebugLine(vec3(mat * vec4(0.0f, 0.0f, 0.0f, 1.0f)), vec3(mat * vec4(0.1f, 0.0f, 0.0f, 1.0f)), vec3(1.0f, 0.0f, 0.0f)); //x
    Renderer::debug.renderDebugLine(vec3(mat * vec4(0.0f, 0.0f, 0.0f, 1.0f)), vec3(mat * vec4(0.0f, 0.1f, 0.0f, 1.0f)), vec3(0.0f, 1.0f, 0.0f)); //y
    Renderer::debug.renderDebugLine(vec3(mat * vec4(0.0f, 0.0f, 0.0f, 1.0f)), vec3(mat * vec4(0.0f, 0.0f, 0.1f, 1.0f)), vec3(0.0f, 0.0f, 1.0f)); //z
}

void Bone::debugDraw(const mat4 &parentTransform) const {
    //Renderer::debug.renderDebugSphere(this->position, 0.1f, vec3(1.0f, 0.0f, 0.0f));

    //Renderer::debug.renderDebugSphere(vec3(getLocalTransform() * vec4(0.0f, 0.0f, 0.0f, 1.0f)), 0.1f, vec3(1.0f, 0.0f, 0.0f));

    mat4 local = parentTransform * getLocalTransform();

    renderMat4(local);

    for (size_t i = 0; i < children.size(); i++) {
        Renderer::debug.renderDebugLine(vec3(local * vec4(0.0f, 0.0f, 0.0f, 1.0f)), vec3(local * vec4(children[i].position, 1.0f)), vec3(1.0f, 1.0f, 1.0f));


        children[i].debugDraw(local);
    }
}