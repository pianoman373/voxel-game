#pragma once

#include "json.hpp"

class Settings {
public:
    static int shadow_resolution;
    static bool shadows;

    static void load(json j);
};
