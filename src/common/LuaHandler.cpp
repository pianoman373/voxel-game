#include "common/LuaHandler.hpp"

#include "common/Block.hpp"
#include "common/Entity.hpp"
#include "util/Noise.hpp"
#include "util/SimplexNoise.hpp"
#include "client/Client.hpp"

#include "util/Input.hpp"
#include "rendering/GuiRenderer.hpp"
#include "client/Window.hpp"
#include "rendering/Resources.hpp"

#include <imgui.h>

#include <fstream>

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
     state["api"]["renderSpriteColor"] = [](float positionX, float positionY, float sizeX, float sizeY, float colorR, float colorG, float colorB, float colorA) {
         GuiRenderer::renderSprite({positionX, positionY}, {sizeX, sizeY}, {colorR, colorG, colorB, colorA});
     };
     state["api"]["renderSprite"] = [](float positionX, float positionY, float sizeX, float sizeY, float colorR, float colorG, float colorB, float colorA) {
         GuiRenderer::renderSprite({positionX, positionY}, {sizeX, sizeY},{colorR, colorG, colorB, colorA});
     };
     state["api"]["renderSpriteTexture"] = [](float positionX, float positionY, float sizeX, float sizeY, float uvU1, float uvV1, float uvU2, float uvV2, Texture &tex) {
         GuiRenderer::renderSprite({positionX, positionY}, {sizeX, sizeY}, {uvU1, uvV1, uvU2, uvV2},{1.0f, 1.0f, 1.0f, 1.0f}, tex);
     };
     state["api"]["renderText"] = [&](float x, float y, std::string text, const Font &font, float r, float g, float b, float a) {
         GuiRenderer::renderText({x, y}, text, font, {r, g, b, a});
     };
     state["api"]["renderBlockItem"] = [&](int blockID, float x, float y, float size) {
         client.itemRenderer.renderBlockItem(client.world.blockRegistry.getBlock(blockID), x, y, size);
     };
     state["api"]["renderDebugLine"] = [&](const vec3 &v1, const vec3 &v2, const vec3 &color) {
         Renderer::debug.renderDebugLine(v1, v2, color);
     };
     state["api"]["renderDebugAABB"] = [&](const vec3 &v1, const vec3 &v2, const vec3 &color) {
         Renderer::debug.renderDebugAABB(v1, v2, color);
     };

     state["api"]["getTexture"] = [&](std::string path) {
         return client.registry.getTexture(formatModPath(path));
     };
     state["api"]["getFont"] = [&](std::string path) {
         return client.registry.getFont(formatModPath(path));
     };
     state["api"]["getTime"] = [&]() {
         return Window::getTime();
     };
     state["api"]["getDelta"] = [&]() {
         return ImGui::GetIO().DeltaTime;
     };
     state["api"]["getFramerate"] = [&]() {
         return ImGui::GetIO().Framerate;
     };
    

     state["api"]["input"] = state.create_table();

     state["api"]["input"]["getScroll"] = []() {
         return (int)Input::getScroll();
     };
     state["api"]["input"]["isKeyDown"] = [](int key) {
         return Input::isKeyDown(key);
     };
     state["api"]["input"]["isKeyPressed"] = [](int key) {
         return Input::isKeyPressed(key);
     };
     state["api"]["input"]["getCharPresses"] = []() {
         return Input::getCharPresses();
     };
     state["api"]["input"]["getCursorPos"] = []() {
         return Input::getCursorPos();
     };
     state["api"]["input"]["isMouseButtonDown"] = [](int button) {
         return Input::isMouseButtonDown(button);
     };
     state["api"]["input"]["isMouseButtonPressed"] = [](int button) {
         return Input::isMouseButtonPressed(button);
     };
     state["api"]["input"]["isMouseGrabbed"] = []() {
         return Input::isMouseGrabbed();
     };
     state["api"]["input"]["setMouseGrabbed"] = [](bool grabbed) {
         Input::setMouseGrabbed(grabbed);
     };

     /* Printable keys */
     state["api"]["input"]["KEY_SPACE"]              = 32;
     state["api"]["input"]["KEY_APOSTROPHE"]         = 39;  /* ' */
     state["api"]["input"]["KEY_COMMA"]              = 44;  /* , */
     state["api"]["input"]["KEY_MINUS"]              = 45;  /* - */
     state["api"]["input"]["KEY_PERIOD"]             = 46;  /* . */
     state["api"]["input"]["KEY_SLASH"]              = 47;  /* / */
     state["api"]["input"]["KEY_0"]                  = 48;
     state["api"]["input"]["KEY_1"]                  = 49;
     state["api"]["input"]["KEY_2"]                  = 50;
     state["api"]["input"]["KEY_3"]                  = 51;
     state["api"]["input"]["KEY_4"]                  = 52;
     state["api"]["input"]["KEY_5"]                  = 53;
     state["api"]["input"]["KEY_6"]                  = 54;
     state["api"]["input"]["KEY_7"]                  = 55;
     state["api"]["input"]["KEY_8"]                  = 56;
     state["api"]["input"]["KEY_9"]                  = 57;
     state["api"]["input"]["KEY_SEMICOLON"]          = 59;  /* ; */
     state["api"]["input"]["KEY_EQUAL"]              = 61;  /* = */
     state["api"]["input"]["KEY_A"]                  = 65;
     state["api"]["input"]["KEY_B"]                  = 66;
     state["api"]["input"]["KEY_C"]                  = 67;
     state["api"]["input"]["KEY_D"]                  = 68;
     state["api"]["input"]["KEY_E"]                  = 69;
     state["api"]["input"]["KEY_F"]                  = 70;
     state["api"]["input"]["KEY_G"]                  = 71;
     state["api"]["input"]["KEY_H"]                  = 72;
     state["api"]["input"]["KEY_I"]                  = 73;
     state["api"]["input"]["KEY_J"]                  = 74;
     state["api"]["input"]["KEY_K"]                  = 75;
     state["api"]["input"]["KEY_L"]                  = 76;
     state["api"]["input"]["KEY_M"]                  = 77;
     state["api"]["input"]["KEY_N"]                  = 78;
     state["api"]["input"]["KEY_O"]                  = 79;
     state["api"]["input"]["KEY_P"]                  = 80;
     state["api"]["input"]["KEY_Q"]                  = 81;
     state["api"]["input"]["KEY_R"]                  = 82;
     state["api"]["input"]["KEY_S"]                  = 83;
     state["api"]["input"]["KEY_T"]                  = 84;
     state["api"]["input"]["KEY_U"]                  = 85;
     state["api"]["input"]["KEY_V"]                  = 86;
     state["api"]["input"]["KEY_W"]                  = 87;
     state["api"]["input"]["KEY_X"]                  = 88;
     state["api"]["input"]["KEY_Y"]                  = 89;
     state["api"]["input"]["KEY_Z"]                  = 90;
     state["api"]["input"]["KEY_LEFT_BRACKET"]       = 91;  /* [ */
     state["api"]["input"]["KEY_BACKSLASH"]          = 92;  /* \ */
     state["api"]["input"]["KEY_RIGHT_BRACKET"]      = 93;  /* ] */
     state["api"]["input"]["KEY_GRAVE_ACCENT"]       = 96;  /* ` */
     state["api"]["input"]["KEY_WORLD_1"]            = 161; /* non-US #1 */
     state["api"]["input"]["KEY_WORLD_2"]            = 162; /* non-US #2 */

     /* Function keys */
     state["api"]["input"]["KEY_ENTER"]              = 257;
     state["api"]["input"]["KEY_ESCAPE"]             = 256;
     state["api"]["input"]["KEY_TAB"]                = 258;
     state["api"]["input"]["KEY_BACKSPACE"]          = 259;
     state["api"]["input"]["KEY_INSERT"]             = 260;
     state["api"]["input"]["KEY_DELETE"]             = 261;
     state["api"]["input"]["KEY_RIGHT"]              = 262;
     state["api"]["input"]["KEY_LEFT"]               = 263;
     state["api"]["input"]["KEY_DOWN"]               = 264;
     state["api"]["input"]["KEY_UP"]                 = 265;
     state["api"]["input"]["KEY_PAGE_UP"]            = 266;
     state["api"]["input"]["KEY_PAGE_DOWN"]          = 267;
     state["api"]["input"]["KEY_HOME"]               = 268;
     state["api"]["input"]["KEY_END"]                = 269;
     state["api"]["input"]["KEY_CAPS_LOCK"]          = 280;
     state["api"]["input"]["KEY_SCROLL_LOCK"]        = 281;
     state["api"]["input"]["KEY_NUM_LOCK"]           = 282;
     state["api"]["input"]["KEY_PRINT_SCREEN"]       = 283;
     state["api"]["input"]["KEY_PAUSE"]              = 284;
     state["api"]["input"]["KEY_F1"]                 = 290;
     state["api"]["input"]["KEY_F2"]                 = 291;
     state["api"]["input"]["KEY_F3"]                 = 292;
     state["api"]["input"]["KEY_F4"]                 = 293;
     state["api"]["input"]["KEY_F5"]                 = 294;
     state["api"]["input"]["KEY_F6"]                 = 295;
     state["api"]["input"]["KEY_F7"]                 = 296;
     state["api"]["input"]["KEY_F8"]                 = 297;
     state["api"]["input"]["KEY_F9"]                 = 298;
     state["api"]["input"]["KEY_F10"]                = 299;
     state["api"]["input"]["KEY_F11"]                = 300;
     state["api"]["input"]["KEY_F12"]                = 301;
     state["api"]["input"]["KEY_F13"]                = 302;
     state["api"]["input"]["KEY_F14"]                = 303;
     state["api"]["input"]["KEY_F15"]                = 304;
     state["api"]["input"]["KEY_F16"]                = 305;
     state["api"]["input"]["KEY_F17"]                = 306;
     state["api"]["input"]["KEY_F18"]                = 307;
     state["api"]["input"]["KEY_F19"]                = 308;
     state["api"]["input"]["KEY_F20"]                = 309;
     state["api"]["input"]["KEY_F21"]                = 310;
     state["api"]["input"]["KEY_F22"]                = 311;
     state["api"]["input"]["KEY_F23"]                = 312;
     state["api"]["input"]["KEY_F24"]                = 313;
     state["api"]["input"]["KEY_F25"]                = 314;
     state["api"]["input"]["KEY_KP_0"]               = 320;
     state["api"]["input"]["KEY_KP_1"]               = 321;
     state["api"]["input"]["KEY_KP_2"]               = 322;
     state["api"]["input"]["KEY_KP_3"]               = 323;
     state["api"]["input"]["KEY_KP_4"]               = 324;
     state["api"]["input"]["KEY_KP_5"]               = 325;
     state["api"]["input"]["KEY_KP_6"]               = 326;
     state["api"]["input"]["KEY_KP_7"]               = 327;
     state["api"]["input"]["KEY_KP_8"]               = 328;
     state["api"]["input"]["KEY_KP_9"]               = 329;
     state["api"]["input"]["KEY_KP_DECIMAL"]         = 330;
     state["api"]["input"]["KEY_KP_DIVIDE"]          = 331;
     state["api"]["input"]["KEY_KP_MULTIPLY"]        = 332;
     state["api"]["input"]["KEY_KP_SUBTRACT"]        = 333;
     state["api"]["input"]["KEY_KP_ADD"]             = 334;
     state["api"]["input"]["KEY_KP_ENTER"]           = 335;
     state["api"]["input"]["KEY_KP_EQUAL"]           = 336;
     state["api"]["input"]["KEY_LEFT_SHIFT"]         = 340;
     state["api"]["input"]["KEY_LEFT_CONTROL"]       = 341;
     state["api"]["input"]["KEY_LEFT_ALT"]           = 342;
     state["api"]["input"]["KEY_LEFT_SUPER"]         = 343;
     state["api"]["input"]["KEY_RIGHT_SHIFT"]        = 344;
     state["api"]["input"]["KEY_RIGHT_CONTROL"]      = 345;
     state["api"]["input"]["KEY_RIGHT_ALT"]          = 346;
     state["api"]["input"]["KEY_RIGHT_SUPER"]        = 347;
     state["api"]["input"]["KEY_MENU"]               = 348;


     state.new_usertype<Font>( "Font",
             "getTextSize", &Font::getTextSize
     );

     state["api"]["connectToServer"] = [&](const std::string &address, int port) {
         client.connectToServer(address, port);
     };
     state["api"]["connectToIntegratedServer"] = [&] () {
         client.connectToIntegratedServer();
     };

     state["api"]["registerPlayerController"] = [&] (sol::table &table) {
         client.player->table = table;
     };

     state.new_usertype<Player>("Player",
        "position", &Player::position,
        "velocity", &Player::velocity,
        "direction", &Player::direction,
        "world", &Player::world
     );

}

void LuaHandler::addCommonFunctions(World &world) {
    state.new_usertype<vec3>("vec3",
        sol::call_constructor, sol::constructors<vec3(), vec3(float, float, float), vec3(const vec3&)>(),
        "x", &vec3::x,
        "y", &vec3::y,
        "z", &vec3::z,
        sol::meta_function::addition, sol::overload(
            sol::resolve<vec3(const vec3&, const vec3&)>(::operator+),
            sol::resolve<vec3(const float, const vec3&)>(::operator+),
            sol::resolve<vec3(const vec3&, const float)>(::operator+)),

        sol::meta_function::subtraction, sol::overload(
            sol::resolve<vec3(const vec3&, const vec3&)>(::operator-),
            sol::resolve<vec3(const float, const vec3&)>(::operator-),
            sol::resolve<vec3(const vec3&, const float)>(::operator-)),
        
        sol::meta_function::multiplication, sol::overload(
            sol::resolve<vec3(const vec3&, const vec3&)>(::operator*),
            sol::resolve<vec3(const float, const vec3&)>(::operator*),
            sol::resolve<vec3(const vec3&, const float)>(::operator*)),

        sol::meta_function::division, sol::overload(
            sol::resolve<vec3(const vec3&, const vec3&)>(::operator/),
            sol::resolve<vec3(const float, const vec3&)>(::operator/),
            sol::resolve<vec3(const vec3&, const float)>(::operator/))
    );

    state.new_usertype<vec3i>("vec3i",
        sol::call_constructor, sol::constructors<vec3i(), vec3i(int, int, int), vec3i(const vec3i&)>(),
        "x", &vec3i::x,
        "y", &vec3i::y,
        "z", &vec3i::z,
        sol::meta_function::addition, sol::overload(
            sol::resolve<vec3i(const vec3i&, const vec3i&)>(::operator+),
            sol::resolve<vec3i(const int, const vec3i&)>(::operator+),
            sol::resolve<vec3i(const vec3i&, const int)>(::operator+)),

        sol::meta_function::subtraction, sol::overload(
            sol::resolve<vec3i(const vec3i&, const vec3i&)>(::operator-),
            sol::resolve<vec3i(const int, const vec3i&)>(::operator-),
            sol::resolve<vec3i(const vec3i&, const int)>(::operator-)),
        
        sol::meta_function::multiplication, sol::overload(
            sol::resolve<vec3i(const vec3i&, const vec3i&)>(::operator*),
            sol::resolve<vec3i(const int, const vec3i&)>(::operator*),
            sol::resolve<vec3i(const vec3i&, const int)>(::operator*)),

        sol::meta_function::division, sol::overload(
            sol::resolve<vec3i(const vec3i&, const vec3i&)>(::operator/),
            sol::resolve<vec3i(const int, const vec3i&)>(::operator/),
            sol::resolve<vec3i(const vec3i&, const int)>(::operator/))
    );

    state.new_usertype<vec2>("vec2",
        sol::call_constructor, sol::constructors<vec2(), vec2(float, float), vec2(const vec2&)>(),
        "x", &vec2::x,
        "y", &vec2::y,
        sol::meta_function::addition, sol::overload(
            sol::resolve<vec2(const vec2&, const vec2&)>(::operator+),
            sol::resolve<vec2(const float, const vec2&)>(::operator+),
            sol::resolve<vec2(const vec2&, const float)>(::operator+)),

        sol::meta_function::subtraction, sol::overload(
            sol::resolve<vec2(const vec2&, const vec2&)>(::operator-),
            sol::resolve<vec2(const float, const vec2&)>(::operator-),
            sol::resolve<vec2(const vec2&, const float)>(::operator-)),
        
        sol::meta_function::multiplication, sol::overload(
            sol::resolve<vec2(const vec2&, const vec2&)>(::operator*),
            sol::resolve<vec2(const float, const vec2&)>(::operator*),
            sol::resolve<vec2(const vec2&, const float)>(::operator*)),

        sol::meta_function::division, sol::overload(
            sol::resolve<vec2(const vec2&, const vec2&)>(::operator/),
            sol::resolve<vec2(const float, const vec2&)>(::operator/),
            sol::resolve<vec2(const vec2&, const float)>(::operator/))
    );

    state["vecmath"] = state.create_table();
    state["vecmath"]["normalize"] = sol::resolve<vec3(const vec3&)>(::normalize);
    state["vecmath"]["cross"] = sol::resolve<vec3(const vec3&, const vec3&)>(::cross);


     state.get<sol::table>("api").set_function("registerEventHandler", [&](std::string name, sol::function cb)
     {
         eventHandlers[name].push_back(cb);
     });

     state.new_usertype<Chunk>( "Chunk",
             // typical member function that returns a variable
                                    "setBlockRaw", &Chunk::setBlockRaw
     );

     state.new_usertype<World>( "World",
             // typical member function that returns a variable
                                    "setBlockRaw", &World::setBlockRaw,
                                    "getBlock", &World::getBlock,
                                    "setBlock", &World::setBlock,
                                    "breakBlock", &World::breakBlock,
                                    "raycastBlocks", &World::raycastBlocks,
                                    "spawnEntity", &World::spawnEntity
     );

     state.new_usertype<Block>( "Block",
             // typical member function that returns a variable
                                    "getID", &Block::getID,
                                    "table", &Block::table,
                                    "getStringID", &Block::getStringID
     );

     state.new_usertype<Entity>("Entity",
        "x", &Entity::x,
        "y", &Entity::y,
        "z", &Entity::z,
        "xVelocity", &Entity::xVelocity,
        "yVelocity", &Entity::yVelocity,
        "zVelocity", &Entity::zVelocity,
        "xDirection", &Entity::xDirection,
        "yDirection", &Entity::yDirection,
        "zDirection", &Entity::zDirection,
        "width", &Entity::width,
        "height", &Entity::height,
        "depth", &Entity::depth,
        "collidedHorizontal", &Entity::collidedHorizontal,
        "onGround", &Entity::onGround
     );

     state["api"]["registerEntity"] = [&](const std::string &id, sol::table entity) {
         world.entityRegistry.registerEntity(*this, id, entity);
     };

     state["api"]["registerBlock"] = [&](const std::string &id, sol::table block) {
         world.blockRegistry.registerBlockLua(id, block);
     };
     state["api"]["getBlock"] = [&](const std::string &id) {
         return world.blockRegistry.getBlock(id);
     };

     state["require"] = [&] (const std::string &file) {
         return state.require_file(file, formatModPath(file));
     };

     state.do_string(R"(
        ffi = require("ffi")
        ffi.cdef[[
        unsigned char* getChunkData(void *c)
        ]]
        internal = ffi.C
     )");
}

int my_exception_handler(lua_State* L, sol::optional<const std::exception&> maybe_exception, sol::string_view description) {
	// L is the lua state, which you can wrap in a state_view if necessary
	// maybe_exception will contain exception, if it exists
	// description will either be the what() of the exception or a description saying that we hit the general-case catch(...)
	std::cout << "An exception occurred in a function, here's what it says ";
	if (maybe_exception) {
		std::cout << "(straight from the exception): ";
		const std::exception& ex = *maybe_exception;
		std::cout << ex.what() << std::endl;
	}
	else {
		std::cout << "(from the description parameter): ";
		std::cout.write(description.data(), description.size());
		std::cout << std::endl;
	}

	// you must push 1 element onto the stack to be 
	// transported through as the error object in Lua
	// note that Lua -- and 99.5% of all Lua users and libraries -- expects a string
	// so we push a single string (in our case, the description of the error)
	return sol::stack::push(L, description);
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
                         sol::lib::string,
                         sol::lib::table,
                         sol::lib::utf8,
                         sol::lib::ffi,
                         sol::lib::jit
    );
    state.set_exception_handler(&my_exception_handler);

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

        try {
            state.safe_script_file(mod.rootFolder.appendFile("init.lua"));
        }
        catch( const sol::error& e ) {
		    std::cout << e.what() << std::endl;
	    }
    }
}

void LuaHandler::runClientScripts() {
    for (int i = 0; i < loadedMods.size(); i++) {
        LuaMod &mod = loadedMods[i];

        try {
            state.safe_script_file(mod.rootFolder.appendFile("init_client.lua"));
        }
        catch( const sol::error& e ) {
		    std::cout << e.what() << std::endl;
	    }
        
    }
}

void LuaHandler::runScript(std::string script) {
    try {
        state.safe_script_file(script);
    }
    catch( const sol::error& e ) {
        std::cout << e.what() << std::endl;
    }
}


void LuaHandler::registerEventHandler(std::string name, sol::function cb) {

}

std::string LuaHandler::formatModPath(const std::string &input) {
    std::string delimiter = ":";
    std::string modName = input.substr(0, input.find(delimiter));
    std::string modPath = input.substr(input.find(delimiter)+1, input.size());

    return "mods/" + modName + "/" + modPath;
}