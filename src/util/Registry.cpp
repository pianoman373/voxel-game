#include "util/Registry.hpp"
#include "common/LuaHandler.hpp"

void Registry::registerFont(std::string path) {
    Font f;
    f.loadFromFile(path, 32.0f);

    loadedFonts[path] = f;
}

Font& Registry::getFont(std::string path) {
    return loadedFonts[path];
}