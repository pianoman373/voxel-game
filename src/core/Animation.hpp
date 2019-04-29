#pragma once

#include "Math.hpp"

#include <vector>
#include <map>

class Bone;

struct Keyframe {
    float time;
    Transform transform;
};

class Animation {
public:
    std::map<std::string, std::vector<Keyframe>> keyframes;

    float length = 0.0f;

    float currentPosition = 0.0f;

    void applyToSkeleton(float time, Bone &skeleton);

    void animateLooped(float delta, Bone &skeleton);
};