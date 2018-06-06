#pragma once

#include "json.hpp"


class Settings {
public:
    static int shadow_resolution;
    static int render_distance;
    static bool shadows;
    static float fov;
    static bool fancy_graphics;

    static void load(json j);
};
