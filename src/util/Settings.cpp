#include "util/Settings.hpp"

#include <iostream>
#include <fstream>

void Settings::load(std::string file) {
    std::ifstream i(file);
    json j;

    if (i) {
        i >> j;
    }


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

    if (j["vsync"].is_boolean())
        vsync = j["vsync"];

    if (j["resolution_scale"].is_number())
        resolution_scale = j["resolution_scale"];
}
