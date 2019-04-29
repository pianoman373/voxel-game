#pragma once

#include <map>
#include <string>

#include "core/Texture.hpp"
#include "core/Font.hpp"

class Registry {
private:
    std::map<std::string, Font> loadedFonts;


public:

    void registerFont(std::string path);

    Font &getFont(std::string path);
};
