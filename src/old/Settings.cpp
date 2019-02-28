#include "Settings.hpp"

#include <iostream>

int Settings::shadow_resolution = 2048;
bool Settings::shadows = true;
int Settings::render_distance = 256;
float Settings::fov = 70.0f;
bool Settings::fancy_graphics = true;

void Settings::load(json j) {
    if (j["shadow_resolution"].is_number())
        shadow_resolution = j["shadow_resolution"];

    if (j["shadows"].is_boolean())
        shadows = j["shadows"];

    if (j["render_distance"].is_number())
        render_distance = j["render_distance"];

    if (j["fov"].is_number())
        fov = j["fov"];

    if (j["fancy_graphics"].is_boolean())
        fancy_graphics = j["fancy_graphics"];

    std::cout << shadow_resolution << std::endl;
}
