#include "Noise.hpp"
#include "SimplexNoise.hpp"

#include <math.h>

float Noise::noise(float x, float y, int octaves, float frequency, float persistence) {
    float total = 0.0;
    float maxAmplitude = 0.0;
    float amplitude = 1.0;
    for (int i = 0; i < octaves; i++) {
        total += SimplexNoise::noise(x * frequency, y * frequency) * amplitude;
        frequency *= 2.0;
        maxAmplitude += amplitude;
        amplitude *= persistence;
    }
    return total / maxAmplitude;
}

float Noise::ridgedNoise(float x, float y, int octaves, float frequency, float persistence) {
    float total = 0.0f;
    float maxAmplitude = 0.0f;
    float amplitude = 1.0f;
    for (int i = 0; i < octaves; i++) {
        total += ((1.0f - fabs(SimplexNoise::noise(x * frequency, y * frequency))) * 2.0f - 1.0f) * amplitude;
        frequency *= 2.0f;
        maxAmplitude += amplitude;
        amplitude *= persistence;
    }

    return total / maxAmplitude;
}