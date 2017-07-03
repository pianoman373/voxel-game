#include "Util.hpp"

#include <gtc/noise.hpp>
#include <fstream>
#include "MeshFactory.hpp"


float Util::noise(vec2 position, int octaves, float frequency, float persistence) {
    float total = 0.0;
    float maxAmplitude = 0.0;
    float amplitude = 1.0;
    for (int i = 0; i < octaves; i++) {
        total += glm::simplex(glm::vec2(position.x * frequency, position.y * frequency)) * amplitude;
        frequency *= 2.0;
        maxAmplitude += amplitude;
        amplitude *= persistence;
    }
    return total / maxAmplitude;
}

float Util::ridgedNoise(vec2 position, int octaves, float frequency, float persistence) {
    float total = 0.0f;
    float maxAmplitude = 0.0f;
    float amplitude = 1.0f;
    for (int i = 0; i < octaves; i++) {
        total += ((1.0f - fabs(glm::simplex(glm::vec2(position.x * frequency, position.y * frequency)))) * 2.0f - 1.0f) * amplitude;
        frequency *= 2.0f;
        maxAmplitude += amplitude;
        amplitude *= persistence;
    }

    return total / maxAmplitude;
}

json Util::loadJsonFile(std::string file) {
    // read a JSON file
    std::ifstream i(file);
    json j;

    if (i) {
        i >> j;
    }

    return j;
}

std::string Util::loadFile(std::string file) {
    std::ifstream i(file);
    std::string s;
    i >> s;

    return s;
}
