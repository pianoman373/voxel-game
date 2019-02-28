#include "LuaHandler.hpp"

#include "Block.hpp"
#include "Noise.hpp"

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
    state["api"]["ridgedNoise"] = Noise::ridgedNoise;
}

void LuaHandler::runScripts() {
    state.script_file("init.lua", sol::script_default_on_error);
}