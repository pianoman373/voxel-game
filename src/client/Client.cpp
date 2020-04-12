#include "client/Client.hpp"

#include "client/NetworkManagerClient.hpp"
#include "util/Noise.hpp"
#include "client/Window.hpp"
#include "rendering/Renderer.hpp"
#include "rendering/Primitives.hpp"
#include "util/Input.hpp"
#include "rendering/GuiRenderer.hpp"
#include "util/Packet.hpp"
#include "common/Block.hpp"
#include "util/Stopwatch.hpp"
#include "util/Job.hpp"

#include "optick.h"

#include <cstring>
#include <enet/enet.h>
#include <typeinfo>
#include <typeindex>
#include <strstream>

Client::Client(): network(*this), worldRenderer(world, *this), itemRenderer(*this), world(*this, lua) {
    rleCache = new uint8_t[16*16*256*5];

    player = new Player(&world, *this);
}

Client::~Client() {
    delete[] rleCache;
}

static std::vector<vec2i> chunksToDelete;

void Client::manageChunks(vec2i playerChunkPosition) {
    static vec2i lastPlayerChunkPosition;
    static bool first = true;

    OPTICK_EVENT("Manage Chunks");

    if (playerChunkPosition == lastPlayerChunkPosition && !first)
        return;
    

    first = false;
    lastPlayerChunkPosition = playerChunkPosition;
    

    //chunk deletion
    {
        OPTICK_EVENT("Chunk deletion");

        std::unordered_map<vec2i, std::shared_ptr<Chunk>> &chunks = world.getChunks();
        

        for (auto &i : chunks) {
            Chunk *chunk = i.second.get();


            if (abs(playerChunkPosition.x - chunk->chunk_x) > settings.render_distance || abs(playerChunkPosition.y - chunk->chunk_z) > settings.render_distance) {
                chunksToDelete.push_back(vec2i(chunk->chunk_x, chunk->chunk_z));
            }
        }

        for (int i = 0; i < chunksToDelete.size(); i++) {
            world.deleteChunk(chunksToDelete[i].x, chunksToDelete[i].y);
            worldRenderer.deleteChunkRenderer(chunksToDelete[i].x, chunksToDelete[i].y);
        }
        chunksToDelete.clear();
    }

    {
        OPTICK_EVENT("Chunk addition");
        //chunk addition
        for (int x = -settings.render_distance; x < settings.render_distance+1; x++) {
            for (int z = -settings.render_distance; z < settings.render_distance+1; z++) {
                vec2i chunkPos = vec2i(x, z) + playerChunkPosition;

                if (!world.chunkExists(chunkPos.x, chunkPos.y)) {
                    if (expectedChunks.find({chunkPos.x, chunkPos.y}) == expectedChunks.end()) {
                        requestChunkFromServer(chunkPos.x, chunkPos.y);

                        expectedChunks[{chunkPos.x, chunkPos.y}] = 1;
                    }
                }
            }
        }
    }
}

void Client::requestChunkFromServer(int x, int z) {
    Packet p;
    uint16_t packetID = 3;

    p << packetID << x << z;

    network.sendPacket(p);
}

void Client::receivePackets() {
    OPTICK_EVENT();

    {
        OPTICK_EVENT("process network events");
        network.processEvents();
    }
    

    static long bytesReceived = 0;

    int i = 0;

    while (!network.isEmpty() && !abortNetworkProcessing) {
        i++;
        Packet p = network.getPacket();

        uint16_t packetID;
        p >> packetID;

        if (packetID == 0) {

        }
        if (packetID == 1) { 
            std::string s;
            p >> s;

            std::cout << "(Client) Message from server : " << s << std::endl;
        }
        if (packetID == 2) { //player position data
            uint16_t playerID;
            float x, y, z;

            p >> playerID >> x >> y >> z;

            //std::cout << "received position data for player (" << playerID << ") " << x << ", " << y << ", " << z << std::endl;

            playerPositions[playerID] = vec3(x, y, z);
        }
        if (packetID == 3) { //chunk data
            OPTICK_EVENT("Process Chunk Packet");
            int x, z;
            uint16_t length;

            p >> x >> z >> length;

            //std::cout << "(Client) Received chunk at: " << x << ", " << z << std::endl;
            bytesReceived += length;
            //std::cout << "Received a total of " << bytesReceived << " bytes." << std::endl;

            std::shared_ptr<Chunk> chunk;
            {
                OPTICK_EVENT("getChunk");
                chunk = world.getChunk(x, z);
            }
            
            {
                OPTICK_EVENT("Release Data");
                p.releaseData(rleCache, length);
            }
            

            {
                OPTICK_EVENT("RLE decompression");
                chunk->unSerialize(rleCache, length);
            }
            

            chunk->isDirty = true;

            worldRenderer.getChunkRenderer(x, z);

            expectedChunks.erase({x, z});
        }
        if (packetID == 4) {
            int x, y, z, blockID;

            p >> x >> y >> z >> blockID;

            world.setBlockNoPacket(x, y, z, world.blockRegistry.getBlock(blockID));
        }
    }
}

void Client::init() {
    OPTICK_SET_MEMORY_ALLOCATOR(
        [](size_t size) -> void* { return operator new(size); }, 
        [](void* p) { operator delete(p); }, 
        []() { /* Do some TLS initialization here if needed */ }
    );

    std::cout << "running client" << std::endl;

    settings.load("settings.json");

    Window::create({1400, 800}, "Cube Quest", false, settings.vsync);

    Renderer::init(1400*settings.resolution_scale, 800*settings.resolution_scale);
    // add post processing effects
    if (settings.fancy_graphics) {
        Renderer::postProcessingStack.push_back(std::shared_ptr<PostProcessor>(new SsaoPostProcessor())); // SSAO
        //Renderer::postProcessingStack.push_back(std::shared_ptr<PostProcessor>(new BloomPostProcessor())); // Bloom
    }
    
    Renderer::postProcessingStack.push_back(std::shared_ptr<PostProcessor>(new TonemapPostProcessor())); // Tonemapping

    FogPostProcessor *fog = new FogPostProcessor();
    fog->fogOuter = settings.render_distance*16.0f;
    fog->fogInner = fog->fogOuter * 0.8f;
    Renderer::postProcessingStack.push_back(std::shared_ptr<PostProcessor>(fog)); // Fog

    if (settings.fancy_graphics) {
        Renderer::postProcessingStack.push_back(std::shared_ptr<PostProcessor>(new FxaaPostProcessor())); // FXAA
    }

    itemRenderer.init();

    lua.init();
    lua.addCommonFunctions(world);
    lua.addClientSideFunctions(*this);
    lua.runScripts();
    lua.runClientScripts();

    world.blockRegistry.clientInit(*this);
}

void Client::connectToServer(std::string address, int port) {
    std::cout << "connecting to server" << std::endl;

    network.init(address, port);
    
    worldRenderer.init();

    player->position = vec3(16*16, 250, 18*16);
    vec2i playerChunkPosition = vec2i((int)player->position.x >> 4, (int)player->position.z >> 4);
    manageChunks(playerChunkPosition);

    inGame = true;
}

void Client::connectToIntegratedServer(const std::string &folder) {
    integratedServer = new Server(folder);

    serverThread = new std::thread([&]() {
        OPTICK_THREAD("Integrated Server")
        integratedServer->run(running, 3737);
    });

    connectToServer("localhost", 3737);
}

void Client::update(float delta) {
    OPTICK_EVENT();
    if (inGame) {
        vec3 lastPosition = player->position;
        vec2i lastPlayerChunkPosition = vec2i((int)player->position.x >> 4, (int)player->position.z >> 4);

        
        player->update(camera, delta);

        camera.dimensions = {(float)Window::getWindowSize().x, (float)Window::getWindowSize().y};

        if (!(player->position == lastPosition)) {
            Packet p;
            uint16_t packetID = 2;

            p << packetID << player->position.x << player->position.y << player->position.z;

            network.sendPacket(p);
        } 
    }
}

void Client::tick() {
    OPTICK_EVENT();
    if (inGame) {
        world.tick();
    }
}

float accumulator = 0.0f;
float alpha = 0.99f;

void Client::render() {
    OPTICK_EVENT();
    camera.matchWindowResolution();
    Renderer::matchWindowResolution(settings.resolution_scale);

    if (inGame) {
        
        frustum.setupInternals(camera.fov, (float)Window::getWindowSize().x/(float)Window::getWindowSize().y, 0.1f, 1000.0f);
        //if (Input::isKeyDown(Input::KEY_E))
            frustum.updateCamPosition(camera);

       
        worldRenderer.render(camera);
        

        for (auto const& x : playerPositions)
        {
            vec3 position = x.second;
            Renderer::debug.renderDebugAABB(position - vec3(1.0f), position + vec3(1.0f), vec3(1.0f, 0.0f, 0.0f));
        }

        static Job job;

        vec2i playerChunkPosition = vec2i((int)camera.position.x >> 4, (int)camera.position.z >> 4);

        //std::cout << "starting job" << std::endl;
        job.startJob([&] () {
            //if (!(playerChunkPosition == lastPlayerChunkPosition)) {
            
            manageChunks(playerChunkPosition);
            
            //}
            //if (Input::isKeyDown(Input::KEY_Q))
            receivePackets();
            //std::cout << "done receiving packets" << std::endl;
        });
        
        //std::cout << "flushing" << std::endl;
        Renderer::flush(camera, frustum, true);

        //std::cout << "waiting for job to end" << std::endl;
        {
            OPTICK_EVENT("Wait for job");
            abortNetworkProcessing = true;
            job.waitForJob();
            abortNetworkProcessing = false;
        }
        
        //std::cout << "done waiting" << std::endl;

        //thread_obj.join();  

        {
            OPTICK_EVENT("GUI");
            vec2 size = vec2((float)Window::getWindowSize().x, (float)Window::getWindowSize().y);
            lua.pushEvent("gui_ingame", size.x, size.y);
        }
        
    }
    else {
        //main menu
        Renderer::flush(camera);

        vec2 size = vec2((float)Window::getWindowSize().x, (float)Window::getWindowSize().y);
        lua.pushEvent("gui_main_menu", size.x, size.y);
    }
}

void Client::run() {
    OPTICK_THREAD("Main Thread")
    init();

    double t = 0.0;
    double dt = 1.0 / 20.0;

    double currentTime = Window::getTime();
    double accumulator = 0.0;

    while (Window::isOpen()) {
        OPTICK_FRAME("MainThread");

        
        Window::begin();

        double newTime = Window::getTime();
        double frameTime = newTime - currentTime;
        if ( frameTime > 0.25 )
            frameTime = 0.25;
        currentTime = newTime;

        accumulator += frameTime;

        while ( accumulator >= dt )
        {
            tick();

            accumulator -= dt;
        }

        const double alpha = accumulator / dt;


        update(frameTime);

        
        render();

        Window::end();
    }
    running = false;

    std::cout << "Shutting down client" << std::endl;

    enet_deinitialize();

    delete player;

    if (integratedServer) {
        serverThread->join();

        delete integratedServer;
        delete serverThread;
    }
}