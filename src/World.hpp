#pragma once

#include <unordered_map>
#include <vector>
#include <deque>
#include <string>
#include <crucible/Math.hpp>
#include <functional>
#include <mutex>
#include <thread>

#include "ChunkManager.hpp"

class Texture;
class Shader;
class Camera;
class AABB;
class Chunk;
class Material;

struct chunk_position {
    int x;
    int y;
    int z;
};

class World {
private:
    void chunkUpdateThread();

	bool running = true;


	float time = 60 * 2;

    std::thread *thread0;





public:
    ChunkManager manager;

	static constexpr float DAYLIGHT_CYCLE = 60.0f * 20.0f; //20 minutes

	void init(const Camera& camera);

	void shutdown();

    void render(Camera &cam, Material *nearmaterial);

    void update(const Camera &cam, float delta);

	float getTime();

	void setTime(float time);

	vec3 getSunDirection();

	vec3 getSunColor();

	vec3 getAmbient();

    static vec3i worldToChunkPos(vec3 pos);
};
