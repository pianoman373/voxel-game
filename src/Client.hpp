#pragma once

#include <string>

class Frustum;
namespace sf {
    class UdpSocket;
    class IpAddress;
}

class Window;

/**
 * Controls mainly user-side and graphical tasks.
 */
class Client {
private:
    static void init();

    static void handlePackets();

    static void renderGUI(float deltaTime);
public:
    static Frustum frustum;
    static sf::UdpSocket socket;
    static sf::IpAddress connectedServer;
    static Window window;

    static void run(std::string username, std::string ip);

    static void scrollBlocks(int direction);

    static void shutdown();
};
