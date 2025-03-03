// TBGrid.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <chrono>
#include <format>
#include <iostream>
#include <memory>
#include "glm/glm.hpp"
#include "Camera.h"
#include "GraphicsResourceManager.h"
#include "Scene.h"
#include "Stopwatch.h"
#include "UI/UIManager.h"

//Keep these includes (and any other includes that themselves include glfw) down here to avoid macro redefinition warning
#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "Input.h"
#include "GameManager.h"
#include "Renderer.h"

//Declare display and control variables
GLFWwindow* mainWindow = nullptr;
bool exiting = false;
bool showDebugInfo = false;
static bool fullScreen = false;
static glm::ivec2 windowedScreenSize(1024, 768);
static glm::ivec2 fullScreenSize(1920, 1080);
glm::ivec2 screenSize;	//current screen width and height
const static int SCREEN_BPP = 32;
const static int DESIRED_FPS = 60;
int frames = 0;
int fps = 0;
float delta = 0.0f;	//time since last frame

//main objects
Camera mainCamera = Camera(glm::vec3(-5.0f, 5.0f, -5.0f), glm::vec3(1.0f, -1.0f, 1.0f), true, windowedScreenSize);
std::unique_ptr<Scene> scene;
Level testLevel;
std::unique_ptr<GameManager> gameManager;
std::unique_ptr<UIManager> mainUI;
std::unique_ptr<Renderer> renderer;

//glfw callbacks
static void error_callback(int error, const char* description)
{
	fprintf_s(stderr, "Error: %s\n", description);
}

static int exit()
{
	glfwTerminate();
	return 0;
}

static int initGLFW()
{
	std::cout << "Init GLFW... ";
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
	{
		DEBUG_PRINTLN("failed: failed to init glfw");
		return -1;
	}
	//Create a windowed mode window and its OpenGL context. Note: If I hint anything over 3.0 it doesn't like it, but testing shows I'm getting a 4.6 context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	if (fullScreen)
	{
		screenSize = fullScreenSize;
		mainWindow = glfwCreateWindow(screenSize.x, screenSize.y, "TBGrid", glfwGetPrimaryMonitor(), NULL);
	}
	else
	{
		screenSize = windowedScreenSize;
		mainWindow = glfwCreateWindow(screenSize.x, screenSize.y, "TBGrid", NULL, NULL);
	}
	if (!mainWindow)
	{
		glfwTerminate();
		DEBUG_PRINTLN("failed: failed to create window.");
		return -2;
	}
	glfwMakeContextCurrent(mainWindow);

	//input handlers
	glfwSetInputMode(mainWindow, GLFW_CURSOR, mainCamera.followCamera ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(mainWindow, Input::key_callback);
	glfwSetMouseButtonCallback(mainWindow, Input::mouse_button_callback);
	glfwSetScrollCallback(mainWindow, Input::scroll_callback);
	glfwSetCursorPosCallback(mainWindow, Input::cursor_position_callback);
	Input::resetCursorPosition(mainWindow);
	//set up glad
	int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0)
	{
		DEBUG_PRINTLN("failed: GLAD loader initialization error!");
		return -3;
	}
	glfwSwapInterval(1);
	DEBUG_PRINTLN("finished");
	return 1;
}

static void initTest()
{
	//initialisation code for quick testing stuff (e.g. dummy environment)
	//Generally anything intended to be present long-term should be initialised in a more dedicated area
	std::cout << "Test code active" << std::endl;
	testLevel.buildTestLevel();
	scene->replaceSceneContentWithLevel(&testLevel);
}

static void updateCameraAndInput(float delta)
{
	float velocity = 10.0f;	//speed of camera's non-rotational movement / s
	if (mainCamera.followCamera)
	{
		//------------process camera look/movement (follow camera version) --------------
		float rotationScale = -90.0f;
		mainCamera.followBearing = mainCamera.followBearing + (rotationScale * delta * Input::getHorizontalAxis());
		if (mainCamera.followBearing < 0.0f)
		{
			mainCamera.followBearing += 360.0f;
		}
		else if (mainCamera.followBearing > 360.0f)
		{
			mainCamera.followBearing -= 360.0f;
		}

		mainCamera.updateFollowingPosition();
	}
	else
	{
		//------------process camera look/movement (first person perspective version) --------------
		float rotationSpeed = 180.0f;
		mainCamera.updateDirectionFirstPerson(Input::getMouseHorizontalAxis() * rotationSpeed * delta, Input::getMouseVerticalAxis() * rotationSpeed * delta);
		//process camera movement
		mainCamera.pan(delta * velocity * Input::getHorizontalAxis());
		if (Input::getKeyState(GLFW_KEY_SPACE))
		{
			mainCamera.rise(delta * velocity);
		}
		else if (Input::getKeyState(GLFW_KEY_LEFT_SHIFT))
		{
			mainCamera.rise(delta * -velocity);
		}
	}
	//current forward behaviour is independent of camera type
	mainCamera.forward(delta * velocity * Input::getVerticalAxis());

	//handle functionality keys
	if (Input::getKeyDown(GLFW_KEY_O))
	{
		showDebugInfo = !showDebugInfo;
		mainUI->showDebugInfo(showDebugInfo);
		renderer->setDebugDraw(showDebugInfo);
	}
	if (Input::getKeyDown(GLFW_KEY_P))
	{
		if (mainCamera.followCamera)
		{
			//we're swapping to first person mode
			glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			Input::resetCursorPosition(mainWindow);
			mainCamera.switchToFirstPersonMode();
		}
		else
		{
			//we're swapping to follow mode
			glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			mainCamera.switchToFollowMode();
		}
	}

	if (Input::getKeyDown(GLFW_KEY_ESCAPE))
	{
		exiting = true;
	}
}

static void update(const float delta)
{
	gameManager->update(delta);
	mainUI->update(delta);
	scene->update(delta);
	scene->collisionUpdate(delta);

	if (showDebugInfo)
	{
		mainUI->setDebugText(mainCamera.getMainVectorsString(), fps);
	}
}

static bool init(CStopWatch timer)
{
	//returns true if both parts suceeded
	if (initGLFW() >= 1)
	{
		renderer = std::make_unique<Renderer>(mainWindow, screenSize);
		if (renderer->isReady())
		{
			//Now that openGL is loaded, we can initialse some stuff that is dependent on it
			GraphicsResourceManager::getInstance().initialseBasicResources();
			scene = std::make_unique<Scene>(&mainCamera);	//TODO: this is only dependant on openGL being loaded under the current animated mesh testing system
			//init the UI
			mainUI = std::make_unique<UIManager>(screenSize);
			//init the environment and the game
			initTest();
			gameManager = std::make_unique<GameManager>(scene.get(), &testLevel, mainUI.get());
			mainUI->setGameManager(gameManager.get());
			return true;
		}
		else
		{
			DEBUG_PRINTLN("Failed to initialise OpenGL renderer");
		}
	}
	else
	{
		DEBUG_PRINTLN("Failed to initialise GLFW");
	}
	return false;
}

int main()
{
	static CStopWatch fpsTimer;
	static CStopWatch timer;
	exiting = !init(timer);
	char startupTime[32];
	sprintf_s(startupTime, 32, "Total startup time: %f\n", timer.GetElapsedSeconds());
	std::cout << startupTime;
	timer.Reset();
    //main loop
	while (!exiting && !glfwWindowShouldClose(mainWindow))
	{
		delta = timer.GetElapsedSeconds();
		timer.Reset();
		Input::update();
		//handle game state updates
		updateCameraAndInput(delta);
		update(delta);

		//draw everything
		renderer->draw(scene.get(), mainUI.get());
		frames++;
		//No point running at a higher framerate than 60 for the time being, so we give the computer a bit of a break if there is any time left
		//	**Interestingly, with this code enabled FPS tends to hover around 30, but with it enabled we get 60FPS consistently
		//delta = timer.GetElapsedSeconds() * 1000.0f;
		//if (delta < 16.6f)
		//{
		//	long sleep = (long)(16.6f - delta);
		//	std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
		//}
		if (fpsTimer.GetElapsedSeconds() > 1.0f)
		{
			fps = frames;
			frames = 0;
			fpsTimer.Reset();
		}
	}
	return exit();
}