#pragma once

#include <map>
#include <string>

#include <crucible/Texture.hpp>

class TextureRegistry {
private:
    std::map<std::string, Texture> loadedTextures;


public:
    void registerTexture(std::string path);

    Texture &getTexture(std::string path);
};
