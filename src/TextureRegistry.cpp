#include "TextureRegistry.hpp"
#include "LuaHandler.hpp"

void TextureRegistry::registerTexture(std::string path) {
    std::string delimiter = ":";
    std::string modName = path.substr(0, path.find(delimiter));
    std::string modPath = path.substr(path.find(delimiter)+1, path.size());

    Texture t;
    t.load("mods/" + modName + "/resources/" + modPath, true);

    loadedTextures[path] = t;
}

Texture& TextureRegistry::getTexture(std::string path) {


//    if (loadedTextures.find(path) == loadedTextures.end()) {
//
//    }

    return loadedTextures[path];
}