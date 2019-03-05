#pragma once

#include "sol.hpp"

#include <crucible/Path.hpp>

#include <map>

class Client;

struct LuaMod {
    std::string name;
    Path rootFolder;
};

class LuaHandler {
public:
    sol::state state;
    std::vector<LuaMod> loadedMods;

    std::map<std::string, std::vector<sol::function>> eventHandlers;

    LuaHandler();

    void addClientSideFunctions(Client &client);

    void addCommonFunctions();

    void init();

    void runScripts();

    void runScript(std::string script);

    template<typename... Args>
    inline void pushEvent(std::string name, Args&&... args) {
        std::vector<sol::function> callbacks = eventHandlers[name];

        for (int i = 0; i < callbacks.size(); i++) {
            sol::protected_function f = callbacks[i];

            sol::protected_function_result result = f(args...);
            if (result.valid()) {

            }
            else {
                // Call failed
                sol::error err = result;
                std::cout << err.what() << std::endl;
            }
        }
    }

    void registerEventHandler(std::string name, sol::function cb);
};