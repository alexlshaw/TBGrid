#pragma once

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

	//Mouse button tracking (just LMB for now)
	inline bool mouseDown_Left = false;	//True for the first frame in which the mouse is clicked
	inline bool mouseUp_Left = false;		//True for the first frame after the mouse has been released
	inline bool mouseHold_Left = false;	//True in any frame in which the mouse button is pressed
	inline int mouseDownFrameCount_Left = 0;	//Tracker value to determine how many frames the mouse has been held down

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
}