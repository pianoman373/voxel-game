#pragma once

#include <map>
#include <string>

#include <crucible/Texture.hpp>
#include <crucible/Font.hpp>

class Registry {
private:
    std::map<std::string, Texture> loadedTextures;
    std::map<std::string, Font> loadedFonts;


public:
    void registerTexture(std::string path);

    Texture &getTexture(std::string path);

    void registerFont(std::string path);

    Font &getFont(std::string path);
};
