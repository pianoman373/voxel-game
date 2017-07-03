#include "LuaAPI.hpp"
#include "Block.hpp"

extern "C" {
    void lua_registerBlock(int id, const char *name, int texX, int texY) {
        BlockRegistry::registerBlock(id, new SimpleBlock(vec2i(texX, texY), std::string(name)));
    }
}
