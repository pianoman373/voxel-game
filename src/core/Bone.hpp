#pragma once

#include <string>
#include <vector>
#include "Math.hpp"

class Bone {
public:
    std::vector<Bone> children;

    vec3 startingPosition;
    quaternion startingRotation;

    vec3 position;

    quaternion rotation;

    std::string name;

    Bone();

    Bone(const std::string &name, const vec3 position, quaternion rotation);

    Bone &addChild(Bone b);

    Bone *find(const std::string &boneName);

    mat4 getLocalTransform() const;

    mat4 getStartingTransform() const;

    std::vector<mat4> getStartingTransforms() const;

    std::vector<mat4> getSkeletonTransforms() const;

    std::vector<mat4> getSkinningTransforms() const;

    void debugDraw(const mat4 &parentTransform=mat4()) const;
};
