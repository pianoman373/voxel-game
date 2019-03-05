#include "LuaHandler.hpp"

#include "Block.hpp"
#include "Noise.hpp"
#include "SimplexNoise.hpp"
#include "Client.hpp"

#include <crucible/Input.hpp>
#include <crucible/GuiRenderer.hpp>

static void panic_handler(sol::optional<std::string> maybe_msg) {
    std::cerr << "Lua is in a panic state and will now abort() the application" << std::endl;
    if (maybe_msg) {
        const std::string& msg = maybe_msg.value();
        std::cerr << "\terror message: " << msg << std::endl;
    }
    // When this function exits, Lua will exhibit default behavior and abort()
}

LuaHandler::LuaHandler(): state(sol::c_call<decltype(&panic_handler), &panic_handler>) {

}

void LuaHandler::addClientSideFunctions(Client &client) {
    state["api"]["registerBlock"] = BlockRegistry::registerBlockLua;
    state["api"]["renderSpriteColor"] = [](float positionX, float positionY, float sizeX, float sizeY, float colorR, float colorG, float colorB, float colorA) {
        GuiRenderer::renderSprite({positionX, positionY}, {sizeX, sizeY}, {colorR, colorG, colorB, colorA});
    };
    state["api"]["renderSprite"] = [](float positionX, float positionY, float sizeX, float sizeY, float uvU1, float uvV1, float uvU2, float uvV2, float colorR, float colorG, float colorB, float colorA, Texture &tex) {
        GuiRenderer::renderSprite({positionX, positionY}, {sizeX, sizeY}, {uvU1, uvV1, uvU2, uvV2},{colorR, colorG, colorB, colorA}, tex);
    };
    state["api"]["renderSpriteTexture"] = [](float positionX, float positionY, float sizeX, float sizeY, float uvU1, float uvV1, float uvU2, float uvV2, Texture &tex) {
        GuiRenderer::renderSprite({positionX, positionY}, {sizeX, sizeY}, {uvU1, uvV1, uvU2, uvV2},{1.0f, 1.0f, 1.0f, 1.0f}, tex);
    };
    state["api"]["getTexture"] = [&](std::string path) {
        return client.textureRegistry.getTexture(path);
    };
    state["api"]["registerTexture"] = [&](std::string path) {
        client.textureRegistry.registerTexture(path);
    };

    state["api"]["getScroll"] = []() {
        return (int)Input::getScroll();
    };

}

void LuaHandler::addCommonFunctions() {
    state.get<sol::table>("api").set_function("registerEventHandler", [&](std::string name, sol::function cb)
    {
        eventHandlers[name].push_back(cb);
    });
}

void LuaHandler::init() {
    // open some common libraries
    state.open_libraries(sol::lib::base,
                         sol::lib::bit32,
                         sol::lib::coroutine,
                         sol::lib::count,
                         sol::lib::io,
                         sol::lib::math,
                         sol::lib::os,
                         sol::lib::package,
                         sol::lib::string,
                         sol::lib::table,
                         sol::lib::utf8,
                         sol::lib::ffi,
                         sol::lib::jit
    );
    state["api"] = state.create_table();

    Path modsPath = "mods/";
    std::vector<Path> v = Path::listDirectory(modsPath, true);

    for (int i = 0; i < v.size(); i++) {
        loadedMods.push_back({
            v[i],
            modsPath.appendFolder(v[i])
        });

        std::cout << "Loaded mod: " << loadedMods.back().name << " from " << loadedMods.back().rootFolder.toString() << std::endl;
    }
}

void LuaHandler::runScripts() {
    for (int i = 0; i < loadedMods.size(); i++) {
        LuaMod &mod = loadedMods[i];

        state.script_file(mod.rootFolder.appendFile("init.lua"), sol::script_default_on_error);
    }
}

void LuaHandler::runScript(std::string script) {
    state.script_file(script, sol::script_default_on_error);
}


void LuaHandler::registerEventHandler(std::string name, sol::function cb) {

}