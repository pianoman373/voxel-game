#pragma once

#include <map>
#include <string>

#include "rendering/Texture.hpp"
#include "rendering/Font.hpp"

class Registry {
private:
    std::map<std::string, Font> fontRegistry;
    std::map<std::string, Texture> textureRegistry;


public:
    Texture &getTexture(const Path &path);

    Font &getFont(std::string path);
};
