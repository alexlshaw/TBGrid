#pragma once

#include "glm/glm.hpp"
#include "GLFW/glfw3.h"

namespace Input
{
	//As far as I can tell from the docs, GLFW_KEY_LAST is the highest value a glfw key can have
	//So need that + 1 slots (though should be plenty of unused slots in the arrays)
	static bool keyStates[GLFW_KEY_LAST + 1] = { false };
	static bool keyLastStates[GLFW_KEY_LAST + 1] = { false };

	inline double mouseX = 0.0;
	inline double mouseY = 0.0;
	inline double lastMouseX = 0.0;
	inline double lastMouseY = 0.0;
	inline float mouseSensitivity = 0.0005f;	//high values mean more sensitive


	//Mouse button tracking
	const inline int MOUSE_BUTTON_COUNT = GLFW_MOUSE_BUTTON_LAST + 1;
	inline bool mouseDown[MOUSE_BUTTON_COUNT];	//GLFW apparently supports 8 mouse buttons
	inline bool mouseUp[MOUSE_BUTTON_COUNT];
	inline bool mouseHold[MOUSE_BUTTON_COUNT];
	inline int mouseDownFrameCount[MOUSE_BUTTON_COUNT];

	//Temporary action tracking variables before keybind system is implemented
	const static int ACTION_SELECT = 0;
	const static int ACTION_TARGET = 1;

	//returns whatever inputs are mapped to vertical (forward/backward) movement in the range -1.0...1.0
	float getVerticalAxis();

	//returns whatever inputs are mapped to lateral movement in the range -1.0...1.0
	float getHorizontalAxis();

	//returns mouse vertical movement in the range -1.0...1.0
	float getMouseHorizontalAxis();

	//returns mouse vertical movement in the range -1.0...1.0
	float getMouseVerticalAxis();

	//returns true if the key is down in this fame, false otherwise
	bool getKeyState(int glfwKeyCode);

	//returns true in the frame in which the key was pressed, false otherwise
	bool getKeyDown(int glfwKeyCode);

	//returns true in the frame in which the key was released, false otherwise
	bool getKeyUp(int glfwKeyCode);

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

	void resetCursorPosition(GLFWwindow* window);

	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	//updates states of variables that are supposed to track how things were in the previous frame
	void update();

	inline glm::vec2 mouseCoords()
	{
		return glm::vec2(mouseX, mouseY);
	}
}