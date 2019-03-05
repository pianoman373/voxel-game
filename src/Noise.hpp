#pragma once

class Noise {
public:
    /**
     * General purpose simplex noise function.
     *
     * positition is the input.
     * octaves is the detail of the noise (biggest performance hit).
     * frequency is how far apart high parts are generally from low parts.
     * persistence is how much each octave mutates the terrain
     */
    static float noise(float x, float y, int octaves, float frequency, float persistence);

    /**
     * Positive biased noise good for mountains.
     *
     * positition is the input.
     * octaves is the detail of the noise (biggest performance hit).
     * frequency is how far apart high parts are generally from low parts.
     * persistence is how much each octave mutates the terrain
     */
    static float ridgedNoise(float x, float y, int octaves, float frequency, float persistence);
};
