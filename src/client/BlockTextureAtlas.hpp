#pragma once

#include "util/Path.hpp"
#include "rendering/Texture.hpp"

#include <string>
#include <map>

class BlockTextureAtlas {
private:
    TextureArray atlas;

    std::vector<std::string> filePaths;
    std::map<std::string, int> ids;

public:
    int registerTexture(const std::string &path);

    void buildAtlas();

    TextureArray getTexture();
};