#include "Input.h"

using namespace Input;

float Input::getVerticalAxis()
{
	float up = keyStates[GLFW_KEY_W] ? 1.0f : 0.0f;
	float down = keyStates[GLFW_KEY_S] ? -1.0f : 0.0f;
	return up + down;
}

float Input::getHorizontalAxis()
{
	float left = keyStates[GLFW_KEY_A] ? 1.0f : 0.0f;
	float right = keyStates[GLFW_KEY_D] ? -1.0f : 0.0f;
	return left + right;
}

float Input::getMouseVerticalAxis()
{
	float delta = (float)(lastMouseY - mouseY);
	delta *= mouseSensitivity;
	if (delta < -1.0f) delta = -1.0f;
	if (delta > 1.0f) delta = 1.0f;
	return delta;
}

float Input::getMouseHorizontalAxis()
{
	float delta = (float)(lastMouseX - mouseX);
	delta *= mouseSensitivity;
	if (delta < -1.0f) delta = -1.0f;
	if (delta > 1.0f) delta = 1.0f;
	return delta;
}

bool Input::getKeyState(int glfwKeyCode)
{
	return keyStates[glfwKeyCode];
}

bool Input::getKeyDown(int glfwKeyCode)
{
	return keyStates[glfwKeyCode] && !keyLastStates[glfwKeyCode];
}

bool Input::getKeyUp(int glfwKeyCode)
{
	return !keyStates[glfwKeyCode] && keyLastStates[glfwKeyCode];
}

void Input::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	bool keyDown = action == GLFW_PRESS || action == GLFW_REPEAT;
	keyStates[key] = keyDown;
}

void Input::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	mouseX = xpos;
	mouseY = ypos;
}

void Input::resetCursorPosition(GLFWwindow* window)
{
	glfwGetCursorPos(window, &mouseX, &mouseY);	//Initialise mouse position values so we don't get a massive jump in first frame
	lastMouseX = mouseX;
	lastMouseY = mouseY;
}

void Input::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		switch (action)
		{
		case GLFW_PRESS:
			mouseDown_Left = true;
			mouseHold_Left = true;
			mouseUp_Left = false;
			break;
		case GLFW_RELEASE:
			mouseDown_Left = false;
			mouseHold_Left = false;
			mouseUp_Left = true;
			mouseDownFrameCount_Left = 0;
			break;
		}
	}
}

void Input::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

}

void Input::update()
{
	for (int i = 0; i < GLFW_KEY_LAST + 1; i++)
	{
		keyLastStates[i] = keyStates[i];
	}
	lastMouseX = mouseX;
	lastMouseY = mouseY;

	//update tracking of whether we have been holding down the mouse button
	mouseDown_Left = false;
	//get the next set of events for the frame ahead
	glfwPollEvents();
}