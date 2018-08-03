#include "Client.hpp"
#include "Settings.hpp"
#include "Util.hpp"
#include "Player.hpp"
#include "Block.hpp"
#include "World.hpp"
#include "Chunk.hpp"

#include "sol.hpp"

#include <crucible/Shader.hpp>
#include <crucible/Texture.hpp>
#include <crucible/Renderer.hpp>
#include <crucible/Window.hpp>
#include <crucible/Frustum.hpp>
#include <glad/glad.h>
#include <crucible/Camera.hpp>
#include <crucible/Input.hpp>
#include <crucible/Math.hpp>
#include <imgui.h>
#include <thread>

static Shader blockShader;
static Shader skyboxShader;
static Texture texture;
static Texture texture_r;
static Texture texture_m;
static Texture texture_e;
static Texture texture_n;
Camera Client::camera;
static Player *player;

Frustum Client::frustum;

static Material nearMaterial;
static Material farMaterial;

World Client::world;

static bool p_open = false;

static std::map<int, vec3> playerPositions;

void Client::renderGUI(float deltaTime) {
    ImGui::SetNextWindowPos(ImVec2(10,10));
    ImGui::SetNextWindowSize(ImVec2(400, 600));

    if (!ImGui::Begin("Example: Fixed Overlay", &p_open, ImVec2(0,0), 0.3f, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings))
    {
        ImGui::End();
        return;
    }
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text("Delta %.3f", deltaTime);
	ImGui::Text("World time %.3f", world.getTime());
    ImGui::Text("PlayerPos: %.2f %.2f %.2f", player->position.x, player->position.y, player->position.z);
    std::string s = "Held block: " + BlockRegistry::getBlock(player->heldBlock)->name;
    ImGui::Text("%s", s.c_str());

    ImGui::End();
}

static const std::vector<vec3> normalLookup = {
        {1.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, -1.0f}
};

static const std::vector<vec3> tangentLookup = {
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, -1.0f},
        {-1.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f}
};

static sol::state luaState;

static int rleEncode(uint8_t *input, int inputLength, uint8_t *output) {
    int i = 0;
    int outputIndex = 0;

    bool first = true;


    uint8_t lastVal = input[i] + 1;
    uint8_t count = 1;

    while (true) {
        uint8_t val = input[i];

        if (val == lastVal && count < 3) {
            count++;
        }
        else {
            if (first) {
                first = false;
            }
            else {
                output[outputIndex] = lastVal;
                output[outputIndex+1] = count;
                outputIndex += 2;
            }

            lastVal = val;
            count = 1;
        }

        i++;
        if (i >= inputLength) {
            output[outputIndex] = lastVal;
            output[outputIndex+1] = count;
            outputIndex += 2;

            return outputIndex;
        }
    }
}

static int rleDecode(uint8_t *input, int inputLength, uint8_t *output) {
    int outputIndex = 0;

    for (int i = 0; i < (inputLength - 1); i += 2) {
        uint8_t number = input[i];
        uint8_t amount = input[i+1];

        for (int j = 0; j < amount; j++) {
            output[outputIndex] = number;
            outputIndex++;
        }
    }

    return outputIndex;
}

void Client::init() {
    uint8_t data[5] = {1, 1, 3, 3, 3};
    uint8_t compressedData[5];
    uint8_t uncompressedData[5];

    int length = rleEncode(data, 5, compressedData);

    int length2 = rleDecode(compressedData, length, uncompressedData);

    for (int i = 0; i < length; i++) {
        std::cout << (int)compressedData[i] << ", ";
    }

    std::cout << std::endl;

    for (int i = 0; i < length2; i++) {
        std::cout << (int)uncompressedData[i] << ", ";
    }

    std::cout << std::endl;

    BlockRegistry::init();

    // open some common libraries
	luaState.open_libraries(sol::lib::base,
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
                         sol::lib::ffi
                         );

    luaState.script_file("api.lua");
    luaState["api"]["registerBlock"] = BlockRegistry::registerBlockLua;

	luaState.script_file("init.lua", sol::script_default_on_error);


    json j = Util::loadJsonFile("settings.json");
    Settings::load(j);

    Renderer::init(Settings::shadows, Settings::shadow_resolution, 1400, 800);
    Renderer::setSun({normalize(vec3(-0.4f, -0.7f, -1.0f)), vec3(1.4f, 1.3f, 1.0f) * 3.0f});

    Renderer::settings.bloom = Settings::fancy_graphics;
    Renderer::settings.bloomStrength = 0.1f;
    Renderer::settings.fxaa = false;
    Renderer::settings.ssao = Settings::fancy_graphics;
    Renderer::settings.ssaoKernelSize = 8;
    Renderer::settings.tonemap = true;
    Renderer::settings.vignette = true;

    blockShader.loadFile("resources/blockShader.vsh", "resources/blockShader.fsh");
    skyboxShader.loadFile("resources/skybox.vsh", "resources/skybox.fsh");
    texture.load("resources/terrain.png", true);
	texture_r.load("resources/terrain_r.png", true);
    texture_m.load("resources/terrain_m.png", true);
    texture_e.load("resources/terrain_e.png", true);
    texture_n.load("resources/terrain_n.png", true);

    nearMaterial.setShader(blockShader);
	nearMaterial.setPBRUniforms(texture, texture_r, texture_m, texture_n);
	nearMaterial.setUniformTexture("emissionTex", texture_e, 5);

	for (int i = 0; i < normalLookup.size(); i++) {
	    nearMaterial.setUniformVec3("normalLookup[" + std::to_string(i) + "]", normalLookup[i]);
	}
    for (int i = 0; i < tangentLookup.size(); i++) {
        nearMaterial.setUniformVec3("tangentLookup[" + std::to_string(i) + "]", tangentLookup[i]);
    }

    farMaterial.setShader(blockShader);
	farMaterial.setPBRUniforms(texture, 0.9f, 0.0f);

    player = new Player(world);
    player->position = vec3(16.0f, 0.0f, 16.0f);

    if (Settings::fancy_graphics) {
        Cubemap cubemap;
        cubemap.loadEquirectangular("resources/skybox.png", 1024);

        Renderer::setSkyboxShader(skyboxShader);
        Renderer::environment = cubemap;
    }

    frustum.setupInternals(Settings::fov, (float)Window::getWindowSize().x/(float)Window::getWindowSize().y, 0.1f, 1000.0f);
}

void Client::run(std::string username, std::string ip) {
    Window::create({1400, 800}, "Voxel Game", false);

    init();
	world.init(camera);

    while (Window::isOpen()) {
        static float deltaTime;
        static float lastFrameTime = Window::getTime();

        float currentFrameTime = Window::getTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        camera.dimensions = {(float)Window::getWindowSize().x, (float)Window::getWindowSize().y};

        Window::begin();
        scrollBlocks(Input::getScroll());

        player->update(camera, deltaTime);


        //if (Input::isKeyDown(Input::KEY_LEFT_CONTROL))
            frustum.updateCamPosition(camera);
        //frustum.renderDebug();

		world.update(camera, deltaTime);
        Renderer::setSun({ world.getSunDirection(), world.getSunColor() });
		Renderer::ambient = world.getAmbient();

        world.render(camera, &nearMaterial);

        renderGUI(deltaTime);


        //render scene and update window
       Renderer::flush(camera, frustum);

        Window::end();
    }

    shutdown();
}

void Client::scrollBlocks(int direction) {
    player->heldBlock += direction;

    if (player->heldBlock >= BlockRegistry::registeredBlocks()) {
        player->heldBlock = 1;
    }
    else if (player->heldBlock < 1) {
        player->heldBlock = BlockRegistry::registeredBlocks() - 1;
    }
}

void Client::shutdown() {
	world.shutdown();
    Window::terminate();
    exit(0);
}
