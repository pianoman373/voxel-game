#include "util/Registry.hpp"
#include "common/LuaHandler.hpp"

Texture &Registry::getTexture(const Path &path) {
    if (textureRegistry.find(path) == textureRegistry.end()) {
        std::cout << "loading texture: " << path << std::endl;

        Texture texture;
        texture.load(path);

        textureRegistry.insert(std::make_pair(path, texture));
    }
    return textureRegistry.at(path);
}

Font& Registry::getFont(std::string path) {
    if (fontRegistry.find(path) == fontRegistry.end()) {
        std::cout << "loading font: " << path << std::endl;

        Font font;
        font.loadFromFile(path, 32.0f);

        fontRegistry.insert(std::make_pair(path, font));
    }
    return fontRegistry.at(path);
}