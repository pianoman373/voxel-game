#include "Client.hpp"
#include "Settings.hpp"
#include "Util.hpp"
#include "Player.hpp"
#include "Console.hpp"
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
static Shader blockShaderFar;
static Shader skyboxShader;
static Texture texture;
static Texture texture_r;
static Camera camera;
static Player *player;
//static Console console;

Frustum Client::frustum;

static Material nearMaterial;
static Material farMaterial;

World Client::world;
sol::state Client::luaState;

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

static void registerBlockNew(int id) {
	std::cout << "registering block id " << id << std::endl;

	BlockRegistry::registerBlock(id, new LuaBlock(id));
}

void Client::init() {
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
	//luaState.require_script("inspect", luaState.script_file("inspect.lua"));
	luaState["registerBlockNew"] = registerBlockNew;
	luaState.script_file("api.lua", &sol::default_on_error);
	luaState.script_file("init.lua", &sol::default_on_error);

    json j = Util::loadJsonFile("settings.json");
    Settings::load(j);

    Renderer::init(Settings::shadows, Settings::shadow_resolution, 1400, 800);
    Renderer::setSun({normalize(vec3(-0.4f, -0.7f, -1.0f)), vec3(1.4f, 1.3f, 1.0f) * 3.0f});
    Renderer::settings.bloom = false;
    Renderer::settings.fxaa = true;
    Renderer::settings.ssao = true;
    Renderer::settings.tonemap = true;
    Renderer::settings.vignette = true;

	Renderer::ambient = vec3(0.5, 0.6, 1.0) * 0.1f;

    blockShader.loadFile("resources/blockShader.vsh", "resources/blockShader.fsh");
    //blockShaderFar.loadFile("resources/blockShader.vsh", "resources/blockShaderSimple.fsh");
    skyboxShader.loadFile("resources/skybox.vsh", "resources/skybox.fsh");
    texture.load("resources/terrain.png", true);
	texture_r.load("resources/terrain_r.png", true);

    nearMaterial.setShader(blockShader);
	nearMaterial.setPBRUniforms(texture, 0.9f, 0.0f);

    farMaterial.setShader(blockShader);
	farMaterial.setPBRUniforms(texture, 0.9f, 0.0f);

   

    //glEnable(GL_CULL_FACE);
    //glEnable(GL_DEPTH_TEST);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    player = new Player(world);
    player->position = vec3(16.0f, 100.0f, 16.0f);

	Cubemap cubemap;
	cubemap.loadEquirectangular("resources/skybox.jpg", 1024);

	Renderer::setSkyboxShader(skyboxShader);
	//Renderer::environment = cubemap;
	Renderer::generateIBLmaps();
}



void Client::run(std::string username, std::string ip) {

    Window::create({1400, 800}, "Voxel Game", false);


    init();
	world.init(camera);

    Renderer::setSun({ world.getSunDirection(), world.getSunColor() });
    Renderer::generateIBLmaps();
    Renderer::environment = Renderer::specular;
    Renderer::setSkyboxShader(Renderer::cubemapShader);


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
        

        vec2i size = Window::getWindowSize();

        if (Input::isKeyDown(GLFW_KEY_F) || true) {
            frustum.setupInternals(Settings::fov, (float) size.x / (float) size.y, 0.1f, 1000.0f);
            float radius = 20.0f;
            //frustum.setupInternalsOrthographic(-radius, radius, -radius, radius, -radius * 8, radius * 8);
            frustum.updateCamPosition(camera);
        }

        //<---===rendering===--->//
        //Common::world.rebuild();
		
		world.update(camera, deltaTime);
		//Common::world.rebuild();
        world.render(camera, &nearMaterial, &farMaterial);
        //frustum.renderDebug();

        for (auto const &ref: playerPositions) {
            vec3 v = ref.second;

            Renderer::debug.renderDebugAABB(v - vec3(0.3f, 0.9f, 0.3f), v + vec3(0.3f, 0.9f, 0.3f), vec3(1.0f, 0.0f, 0.0f));
        }

        renderGUI(deltaTime);

        //render scene and update window
        Renderer::flush(camera);

		

		Renderer::setSun({ world.getSunDirection(), world.getSunColor() });

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
