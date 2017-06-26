#include "Settings.hpp"

int Settings::shadow_resolution = 2048;
bool Settings::shadows = true;

void Settings::load(json j) {
    if (j["shadow_resolution"].is_number())
        shadow_resolution = j["shadow_resolution"];

    if (j["shadows"].is_boolean())
        shadows = j["shadows"];
}
