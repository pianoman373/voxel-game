#pragma once

#include <map>
#include <string>

#include "rendering/Texture.hpp"
#include "rendering/Font.hpp"

class Registry {
private:
    std::map<std::string, Font> loadedFonts;


public:

    void registerFont(std::string path);

    Font &getFont(std::string path);
};
