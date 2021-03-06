#pragma once

#include "sol.hpp"

#include "util/Path.hpp"

#include <map>

class Client;
class World;

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

    void addCommonFunctions(World &world);

    void init();

    void runScripts();

    void runClientScripts();

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

    template<typename... Args>
    static inline void safeCall(sol::protected_function f, Args&&... args) {
        sol::protected_function_result result = f(args...);
        if (result.valid()) {

        }
        else {
            // Call failed
            sol::error err = result;
            std::cout << err.what() << std::endl;
        }
    }

    void registerEventHandler(std::string name, sol::function cb);

    static std::string formatModPath(const std::string &input);
};