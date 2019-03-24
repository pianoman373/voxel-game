#include "Registry.hpp"
#include "LuaHandler.hpp"

void Registry::registerTexture(std::string path) {
    std::string delimiter = ":";
    std::string modName = path.substr(0, path.find(delimiter));
    std::string modPath = path.substr(path.find(delimiter)+1, path.size());

    Texture t;
    t.load("mods/" + modName + "/resources/" + modPath, true);

    loadedTextures[path] = t;
}

Texture& Registry::getTexture(std::string path) {


//    if (loadedTextures.find(path) == loadedTextures.end()) {
//
//    }

    return loadedTextures[path];
}

void Registry::registerFont(std::string path) {
    std::string delimiter = ":";
    std::string modName = path.substr(0, path.find(delimiter));
    std::string modPath = path.substr(path.find(delimiter)+1, path.size());

    Font f;
    f.loadFromFile("mods/" + modName + "/resources/" + modPath, 32.0f);

    loadedFonts[path] = f;
}

Font& Registry::getFont(std::string path) {
    return loadedFonts[path];
}