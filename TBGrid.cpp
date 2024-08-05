// TBGrid.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <chrono>
#include <iostream>
#include <thread>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"

#include "Camera.h"
#include "Input.h"
#include "GraphicsResourceManager.h"
#include "Mesh.h"
#include "MeshTools.h"
#include "Shader.h"
#include "Stopwatch.h"
#include "Text2d.h"
#include "Texture.h"

//Declare display and control variables
GLFWwindow* mainWindow = nullptr;
bool exiting = false;
bool showDebugInfo = false;
static bool fullScreen = false;
static int windowedScreenWidth = 1024;
static int windowedScreenHeight = 768;
static int fullScreenWidth = 1920;
static int fullScreenHeight = 1080;
const static int SCREEN_BPP = 32;
const static int DESIRED_FPS = 60;
int screenWidth, screenHeight;		//current screen width and height
int frames = 0;
int fps = 0;
float delta = 0.0f;	//time since last frame
glm::mat4 orthoProjection;

//main objects
GraphicsResourceManager graphicsResourceManager;
Camera mainCamera = Camera(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(-1.0f, -1.0f, -1.0f), true, windowedScreenWidth, windowedScreenHeight);

//lighting variables
static glm::vec3 Ka = glm::vec3(0.3f, 0.3f, 0.3f);
static glm::vec3 Kd = glm::vec3(1.0f, 1.0f, 1.0f);
static glm::vec3 lightPos = glm::vec3(-1000.0f, 1000.0f, -1000.0f);

//Test objects
Shader* basic;
Mesh* cube;
Texture* testTexture;

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
	std::cout << "Init GL\n";
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
		std::cout <<"Init GL finished\n";
		return true;
	}
	return false;
}

static int initGLFW()
{
	std::cout << "Init GLFW\n";
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
	{
		printf("GLFW init error\n");
		return -1;
	}
	//Create a windowed mode window and its OpenGL context. Note: If I hint anything over 3.0 it doesn't like it, but testing shows I'm getting a 4.6 context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	if (fullScreen)
	{
		screenWidth = fullScreenWidth;
		screenHeight = fullScreenHeight;
		mainWindow = glfwCreateWindow(screenWidth, screenHeight, "TBGrid", glfwGetPrimaryMonitor(), NULL);
	}
	else
	{
		screenWidth = windowedScreenWidth;
		screenHeight = windowedScreenHeight;
		mainWindow = glfwCreateWindow(screenWidth, screenHeight, "TBGrid", NULL, NULL);
	}
	if (!mainWindow)
	{
		glfwTerminate();
		return -2;
	}
	glfwMakeContextCurrent(mainWindow);
	//window size dependent stuff
	orthoProjection = glm::ortho(0.0f, static_cast<float>(screenWidth), 0.0f, static_cast<float>(screenHeight));

	//input handlers
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(mainWindow, Input::key_callback);
	glfwSetMouseButtonCallback(mainWindow, Input::mouse_button_callback);
	glfwSetScrollCallback(mainWindow, Input::scroll_callback);
	glfwSetCursorPosCallback(mainWindow, Input::cursor_position_callback);
	glfwGetCursorPos(mainWindow, &Input::mouseX, &Input::mouseY);	//Initialise mouse position values so we don't get a massive jump in first frame
	Input::lastMouseX = Input::mouseX;
	Input::lastMouseY = Input::mouseY;
	//set up glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("GLAD loader initialization error!\n");
		return -3;
	}
	glfwSwapInterval(1);
	return 1;
}

static void initTest()
{
	//initialisation code for quick testing stuff (e.g. dummy environment)
	//Generally anything intended to be present long-term should be initialised in a more dedicated area
	std::cout << "Test code active\n";

	basic = graphicsResourceManager.loadShader("environment/BasicLit");
	testTexture = graphicsResourceManager.loadTexture("BaseTex");

	std::vector<ColouredVertex> vertices;
	std::vector<unsigned int> indices;

	MeshTools::addCube(&vertices, &indices, glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);
	cube = new Mesh(vertices, indices);
	graphicsResourceManager.addMesh("cube", cube);
}

static void updateCameraAndInput(float delta)
{
	float rotationSpeed = 180.0f;
	float velocity = 10.0f;
	//process camera look
	mainCamera.updateDirection(Input::getMouseHorizontalAxis() * rotationSpeed * delta, Input::getMouseVerticalAxis() * rotationSpeed * delta);
	//process camera movement
	mainCamera.forward(delta * velocity * Input::getVerticalAxis());
	mainCamera.pan(delta * velocity * Input::getHorizontalAxis());
	if (Input::getKeyState(GLFW_KEY_SPACE))
	{
		mainCamera.rise(delta * velocity);
	}
	else if (Input::getKeyState(GLFW_KEY_LEFT_SHIFT))
	{
		mainCamera.rise(delta * -velocity);
	}
	if (Input::getKeyDown(GLFW_KEY_O))
	{
		showDebugInfo = !showDebugInfo;
	}

	if (Input::getKeyDown(GLFW_KEY_ESCAPE))
	{
		exiting = true;
	}
}

static void update(float delta)
{

}

static void draw()
{
	mainCamera.calculateViewMatrix();
	mainCamera.calculateViewMatrixNoPosition();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	testTexture->use();
	
	basic->use();
	int pv = basic->getUniformLocation("projectionViewMatrix");
	basic->setUniform(pv, mainCamera.getProjectionMatrix() * mainCamera.getViewMatrix());
	int mm = basic->getUniformLocation("modelMatrix");
	basic->setUniform(mm, glm::identity<glm::mat4>());
	int nm = basic->getUniformLocation("normalMatrix");
	basic->setUniform(nm, glm::inverseTranspose(glm::mat3(glm::identity<glm::mat4>())));
	int lightIntensity = basic->getUniformLocation("lightIntensity");
	basic->setUniform(lightIntensity, glm::vec3(0.8f, 0.8f, 0.8f));

	int diffuse = basic->getUniformLocation("diffuse");
	basic->setUniform(diffuse, Kd);
	int ambient = basic->getUniformLocation("ambient");
	basic->setUniform(ambient, Ka);
	int lightPosition = basic->getUniformLocation("lightPosition");
	basic->setUniform(lightPosition, glm::normalize(lightPos));
	int t = basic->getUniformLocation("tex");
	basic->setUniform(t, 0);


	cube->draw();

	if (showDebugInfo)
	{
		char camPosition[128];
		char camAngle[64];

		mainCamera.getMainVectorsString(camPosition);
		mainCamera.getAngleString(camAngle);
		printText2D(camPosition, 15, screenHeight - 20, 15);
		printText2D(camAngle, 15, screenHeight - 40, 15);
	}
	
	DEBUG_PRINT_GL_ERRORS();
	glfwSwapBuffers(mainWindow);
}

static bool init(CStopWatch timer)
{
	//returns true if both parts suceeded
	int initGLFWsuccess = initGLFW();
	if (initGLFWsuccess < 1)
	{
		std::cout << "Failed to initialise GLFW\n";
	}
	bool initGLsuccess = initGL();
	if (!initGLsuccess)
	{
		std::cout << "Failed to initialise OpenGL\n";
	}
	const GLubyte* version = glGetString(GL_VERSION);
	if ((initGLFWsuccess == 1) && initGLsuccess && glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
	{
		initText2D(screenWidth, screenHeight);

#ifdef _DEBUG
		initTest();
#endif
		return true;
	}
	else
	{
		std::cout << "Bad Framebuffer status, exiting\n";
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
		glfwPollEvents();
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