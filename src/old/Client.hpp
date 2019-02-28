#pragma once

#include <string>
#include "World.hpp"

class Frustum;

class Window;

/**
 * Controls mainly user-side and graphical tasks.
 */
class Client {
private:
    static void init();

    static void renderGUI(float deltaTime);
public:
    static Frustum frustum;

    static Camera camera;

	static World world;
	//static sol::state luaState;

    static void run(std::string username, std::string ip);

    static void scrollBlocks(int direction);

    static void shutdown();
};
