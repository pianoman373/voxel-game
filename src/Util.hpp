#pragma once

#include "Math.hpp"
#include "Mesh.hpp"
#include "json.hpp"

#include <string>
#include <list>

class Util {
public:
    /**
     * General purpose simplex noise function.
     *
     * positition is the input.
     * octaves is the detail of the noise (biggest performance hit).
     * frequency is how far apart high parts are generally from low parts.
     * persistence is how much each octave mutates the terrain
     */
    static float noise(vec2 position, int octaves, float frequency, float persistence);

    /**
     * Positive biased noise good for mountains.
     *
     * positition is the input.
     * octaves is the detail of the noise (biggest performance hit).
     * frequency is how far apart high parts are generally from low parts.
     * persistence is how much each octave mutates the terrain
     */
    static float ridgedNoise(vec2 position, int octaves, float frequency, float persistence);

    static json loadJsonFile(std::string file);

    static std::string loadFile(std::string file);
};
