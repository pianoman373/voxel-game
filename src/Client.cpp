#include "Client.hpp"
#include "Settings.hpp"
#include "Util.hpp"
#include "Player.hpp"
#include "Block.hpp"
#include "World.hpp"
#include "Chunk.hpp"

#include <crucible/Shader.hpp>
#include <crucible/Texture.hpp>
#include <crucible/Renderer.hpp>
#include <crucible/Window.hpp>
#include <crucible/Frustum.hpp>
#include <glad/glad.h>
#include <crucible/Camera.hpp>
#include <crucible/Input.hpp>
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

void Client::init() {

    BlockRegistry::registerBlock(0, new SimpleBlock({0, 0}, "Air", false));
    BlockRegistry::registerBlock(1, new GrassBlock());
    BlockRegistry::registerBlock(2, new SimpleBlock({1, 0}, "Stone", true));
    BlockRegistry::registerBlock(3, new SimpleBlock({2, 0}, "Dirt", true));
    BlockRegistry::registerBlock(4, new SimpleBlock({0, 1}, "Cobblestone", true));
    BlockRegistry::registerBlock(5, new SimpleBlock({4, 0}, "Planks", true));
    BlockRegistry::registerBlock(6, new SimpleBlock({4, 1}, "Wood", true));
    BlockRegistry::registerBlock(7, new SimpleBlock({4, 3}, "Leaves", false));
    BlockRegistry::registerBlock(8, new SimpleBlock({9, 6}, "Glowstone", true));
    BlockRegistry::registerBlock(9, new SimpleBlock({6, 1}, "Iron", true));
    BlockRegistry::registerBlock(10, new SimpleBlock({7, 1}, "Gold", true));

    json j = Util::loadJsonFile("settings.json");
    Settings::load(j);

    Renderer::init(Settings::shadows, Settings::shadow_resolution, 1400, 800);
    Renderer::setSun({normalize(vec3(-0.4f, -0.7f, -1.0f)), vec3(1.4f, 1.3f, 1.0f) * 3.0f});

    Renderer::settings.bloom = Settings::fancy_graphics;
    Renderer::settings.bloomStrength = 0.1f;
    Renderer::settings.fxaa = Settings::fancy_graphics;
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
