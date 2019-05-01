#pragma once

#include "Math.hpp"
#include <functional>

typedef struct GLFWwindow GLFWwindow;


namespace Input {
	/* Printable keys */
	const int KEY_SPACE              = 32;
	const int KEY_APOSTROPHE         = 39;  /* ' */
	const int KEY_COMMA              = 44;  /* , */
	const int KEY_MINUS              = 45;  /* - */
	const int KEY_PERIOD             = 46;  /* . */
	const int KEY_SLASH              = 47;  /* / */
	const int KEY_0                  = 48;
	const int KEY_1                  = 49;
	const int KEY_2                  = 50;
	const int KEY_3                  = 51;
	const int KEY_4                  = 52;
	const int KEY_5                  = 53;
	const int KEY_6                  = 54;
	const int KEY_7                  = 55;
	const int KEY_8                  = 56;
	const int KEY_9                  = 57;
	const int KEY_SEMICOLON          = 59;  /* ; */
	const int KEY_EQUAL              = 61;  /* = */
	const int KEY_A                  = 65;
	const int KEY_B                  = 66;
	const int KEY_C                  = 67;
	const int KEY_D                  = 68;
	const int KEY_E                  = 69;
	const int KEY_F                  = 70;
	const int KEY_G                  = 71;
	const int KEY_H                  = 72;
	const int KEY_I                  = 73;
	const int KEY_J                  = 74;
	const int KEY_K                  = 75;
	const int KEY_L                  = 76;
	const int KEY_M                  = 77;
	const int KEY_N                  = 78;
	const int KEY_O                  = 79;
	const int KEY_P                  = 80;
	const int KEY_Q                  = 81;
	const int KEY_R                  = 82;
	const int KEY_S                  = 83;
	const int KEY_T                  = 84;
	const int KEY_U                  = 85;
	const int KEY_V                  = 86;
	const int KEY_W                  = 87;
	const int KEY_X                  = 88;
	const int KEY_Y                  = 89;
	const int KEY_Z                  = 90;
	const int KEY_LEFT_BRACKET       = 91;  /* [ */
	const int KEY_BACKSLASH          = 92;  /* \ */
	const int KEY_RIGHT_BRACKET      = 93;  /* ] */
	const int KEY_GRAVE_ACCENT       = 96;  /* ` */
	const int KEY_WORLD_1            = 161; /* non-US #1 */
	const int KEY_WORLD_2            = 162; /* non-US #2 */

	/* Function keys */
	const int KEY_ENTER              = 257;
	const int KEY_ESCAPE             = 256;
	const int KEY_TAB                = 258;
	const int KEY_BACKSPACE          = 259;
	const int KEY_INSERT             = 260;
	const int KEY_DELETE             = 261;
	const int KEY_RIGHT              = 262;
	const int KEY_LEFT               = 263;
	const int KEY_DOWN               = 264;
	const int KEY_UP                 = 265;
	const int KEY_PAGE_UP            = 266;
	const int KEY_PAGE_DOWN          = 267;
	const int KEY_HOME               = 268;
	const int KEY_END                = 269;
	const int KEY_CAPS_LOCK          = 280;
	const int KEY_SCROLL_LOCK        = 281;
	const int KEY_NUM_LOCK           = 282;
	const int KEY_PRINT_SCREEN       = 283;
	const int KEY_PAUSE              = 284;
	const int KEY_F1                 = 290;
	const int KEY_F2                 = 291;
	const int KEY_F3                 = 292;
	const int KEY_F4                 = 293;
	const int KEY_F5                 = 294;
	const int KEY_F6                 = 295;
	const int KEY_F7                 = 296;
	const int KEY_F8                 = 297;
	const int KEY_F9                 = 298;
	const int KEY_F10                = 299;
	const int KEY_F11                = 300;
	const int KEY_F12                = 301;
	const int KEY_F13                = 302;
	const int KEY_F14                = 303;
	const int KEY_F15                = 304;
	const int KEY_F16                = 305;
	const int KEY_F17                = 306;
	const int KEY_F18                = 307;
	const int KEY_F19                = 308;
	const int KEY_F20                = 309;
	const int KEY_F21                = 310;
	const int KEY_F22                = 311;
	const int KEY_F23                = 312;
	const int KEY_F24                = 313;
	const int KEY_F25                = 314;
	const int KEY_KP_0               = 320;
	const int KEY_KP_1               = 321;
	const int KEY_KP_2               = 322;
	const int KEY_KP_3               = 323;
	const int KEY_KP_4               = 324;
	const int KEY_KP_5               = 325;
	const int KEY_KP_6               = 326;
	const int KEY_KP_7               = 327;
	const int KEY_KP_8               = 328;
	const int KEY_KP_9               = 329;
	const int KEY_KP_DECIMAL         = 330;
	const int KEY_KP_DIVIDE          = 331;
	const int KEY_KP_MULTIPLY        = 332;
	const int KEY_KP_SUBTRACT        = 333;
	const int KEY_KP_ADD             = 334;
	const int KEY_KP_ENTER           = 335;
	const int KEY_KP_EQUAL           = 336;
	const int KEY_LEFT_SHIFT         = 340;
	const int KEY_LEFT_CONTROL       = 341;
	const int KEY_LEFT_ALT           = 342;
	const int KEY_LEFT_SUPER         = 343;
	const int KEY_RIGHT_SHIFT        = 344;
	const int KEY_RIGHT_CONTROL      = 345;
	const int KEY_RIGHT_ALT          = 346;
	const int KEY_RIGHT_SUPER        = 347;
	const int KEY_MENU               = 348;

	const int KEY_LAST               = KEY_MENU;

    //private callback functions
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

    void char_callback(GLFWwindow* window, unsigned int codepoint);

    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    void cursor_callback(GLFWwindow* window, double xpos, double ypos);

    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

    bool isKeyDown(int key);

    bool isMouseButtonDown(int button);

    float getScroll();

    vec2 getCursorPos();

    vec2 getLastCursorPos();

    /** Internally used by Window. **/
    void setWindowInstance(GLFWwindow* windowInstance);

    /** Returns time in seconds from program start. **/
    float getTime();

    bool isMouseGrabbed();

	/** Internally used by Window. **/
	void update();

	void registerKeyPressedCallback(std::function<void(int)> callback);

	void registerCharPressedCallback(std::function<void(unsigned int)> callback);
}
