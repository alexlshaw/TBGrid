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
#include "Text2d.h"
#include "UICanvas.h"
#include "UITextElement.h"

//Keep these 3 includes (and any other includes that themselves include glfw) down here to avoid macro redefinition warning
#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "Input.h"
#include "GameManager.h"

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
glm::mat4 orthoProjection;

//main objects
GraphicsResourceManager graphicsResourceManager;
Camera mainCamera = Camera(glm::vec3(-5.0f, 5.0f, -5.0f), glm::vec3(1.0f, -1.0f, 1.0f), true, windowedScreenSize);
Scene scene(&mainCamera);
Level testLevel(&graphicsResourceManager);
GameManager gameManager(&scene, &testLevel);
std::unique_ptr<UICanvas> mainUI;
std::shared_ptr<UITextElement> cameraPositionText;
std::shared_ptr<UITextElement> cameraDirectionText;

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

static bool initGL()
{
	//gl stuff
	DEBUG_PRINT("Init GL... ");
	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_PROGRAM_POINT_SIZE);
	//back to new stuff
	if (glGetError() == GL_NO_ERROR)
	{
		glActiveTexture(GL_TEXTURE0);
		DEBUG_PRINTLN("finished");
		return true;
	}
	DEBUG_PRINTLN("failed");
	return false;
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
	//window size dependent stuff
	orthoProjection = glm::ortho(0.0f, static_cast<float>(screenSize.x), 0.0f, static_cast<float>(screenSize.y));

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

static void initUI()
{
	initText2D();
	mainUI = std::make_unique<UICanvas>(&graphicsResourceManager, orthoProjection);
	//Add elements to the UI
	glm::vec3 debugTextColor(0.1f, 0.3f, 0.1f);
	cameraPositionText = std::make_shared<UITextElement>("CamPosition", glm::vec2(15.0f, screenSize.y - 20.0f), 0.4f, debugTextColor, &(mainUI->defaultFont));
	cameraDirectionText = std::make_shared<UITextElement>("CamDirection", glm::vec2(15.0f, screenSize.y - 40.0f), 0.4f, debugTextColor, &(mainUI->defaultFont));
	mainUI->addElement(cameraPositionText);
	mainUI->addElement(cameraDirectionText);
	cameraPositionText->enabled = showDebugInfo;
	cameraDirectionText->enabled = showDebugInfo;
}

static void initTest()
{
	//initialisation code for quick testing stuff (e.g. dummy environment)
	//Generally anything intended to be present long-term should be initialised in a more dedicated area
	std::cout << "Test code active" << std::endl;
	testLevel.buildTestLevel();
	scene.replaceSceneContentWithLevel(&testLevel);
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
		cameraPositionText->enabled = showDebugInfo;
		cameraDirectionText->enabled = showDebugInfo;
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

static void update(float delta)
{
	gameManager.update(delta);
	scene.update(delta);

	if (showDebugInfo)
	{
		char camPosition[128];
		char camAngle[64];
		mainCamera.getMainVectorsString(camPosition);
		mainCamera.getAngleString(camAngle);
		cameraPositionText->text = camPosition;
		cameraDirectionText->text = camAngle;
	}
}

static void draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	scene.draw();
	mainUI->draw();
	DEBUG_PRINT_GL_ERRORS("TBGrid.cpp: draw()");
	glfwSwapBuffers(mainWindow);
}

static bool init(CStopWatch timer)
{
	//returns true if both parts suceeded
	int initGLFWsuccess = initGLFW();
	if (initGLFWsuccess < 1)
	{
		DEBUG_PRINTLN("Failed to initialise GLFW");
	}
	bool initGLsuccess = initGL();
	if (!initGLsuccess)
	{
		DEBUG_PRINTLN("Failed to initialise OpenGL");
	}
	const GLubyte* version = glGetString(GL_VERSION);
	if ((initGLFWsuccess == 1) && initGLsuccess && glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
	{
		//Now that openGL is loaded, we can initialse some stuff that is dependent on it
		graphicsResourceManager.initialseBasicResources();
		initUI();
#ifdef _DEBUG
		initTest();
#endif
		gameManager.init();
		return true;
	}
	else
	{
		DEBUG_PRINTLN("Bad Framebuffer status, exiting");
		return false;
	}
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
		draw();
		frames++;
		//No point running at a higher framerate than 60 for the time being, so we give the computer a bit of a break if there is any time left
		delta = timer.GetElapsedSeconds() * 1000.0f;
		if (delta < 16.6f)
		{
			long sleep = (long)(16.6f - delta);
			std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
		}
		if (fpsTimer.GetElapsedSeconds() > 1.0f)
		{
			fps = frames;
			frames = 0;
			fpsTimer.Reset();
		}
	}
	return exit();
}