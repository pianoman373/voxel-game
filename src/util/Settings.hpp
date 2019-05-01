#pragma once

#include "json.hpp"
#include <string>


class Settings {
public:
    int shadow_resolution = 2048;
    int render_distance = 256;
    bool shadows = true;
    float fov = 70.0f;
    bool fancy_graphics = true;
    bool vsync = true;
    float resolution_scale = 1.0f;

    void load(std::string file);
};
