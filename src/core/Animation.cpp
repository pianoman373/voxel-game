#include "Animation.hpp"
#include "Bone.hpp"

void Animation::applyToSkeleton(float time, Bone &skeleton) {
    for (auto &i : keyframes) {
        const std::string &name = i.first;
        std::vector<Keyframe> &k = i.second;

        Bone *bone = skeleton.find(name);

        if (bone) {
            for (std::size_t j = 0; j < k.size(); j++) {
                Keyframe keyframe = k[j];

                if (keyframe.time > time) {

                    if (j > 0) {
                        //interpolate between two keyframes
                        Keyframe earlierKeyframe = k[j-1];

                        float amount = (time - earlierKeyframe.time) / (keyframe.time - earlierKeyframe.time);

                        bone->position = lerp(earlierKeyframe.transform.position, keyframe.transform.position, amount);
                        bone->rotation = slerp(earlierKeyframe.transform.rotation, keyframe.transform.rotation, amount);

                    }
                    else {
                        //use only one keyframe
                        bone->position = keyframe.transform.position;
                        bone->rotation = keyframe.transform.rotation;
                    }



                    break;
                }
            }
        }
    }
}

void Animation::animateLooped(float delta, Bone &skeleton) {
    currentPosition += delta;

    if (currentPosition > length) {
        currentPosition -= length;
    }

    applyToSkeleton(currentPosition, skeleton);
}