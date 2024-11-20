#pragma once

#include "Material.h"
#include "Scene.h"
#include "UI/UIManager.h"
#include "glm/glm.hpp"
#include <memory>

#include "glad/gl.h"
#include "GLFW/glfw3.h"

class Renderer
{
private:
	//general drawing stuff
	const GLubyte* version;
	bool readyToDraw;
	bool debugDrawMode = false;
	GLFWwindow* mainWindow;
	glm::ivec2 screenSize;
	Material* activeMaterial = nullptr;
	int materialActivations = 0;
	int drawCount = 0;
	//shadow stuff
	unsigned int depthMapFBO;
	const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
	unsigned int depthMap;	//texture handle
	Shader* depthShader = nullptr;
	int depthShaderProjViewUniform = -1;
	int depthShaderModelUniform = -1;
	void setMaterial(Material* material, Scene* scene);
	void beginFrame(Scene* scene);
	void endFrame(Scene* scene);
	void renderLightingPass(Scene* scene);
	bool initGL();
	void initShadows();
	void drawObject(std::shared_ptr<GameObject> object, Scene* scene);
	void drawObjectLightingPass(std::shared_ptr<GameObject> object, Scene* scene);
	void constructDebugObjects();
	void displayDebug();
	Mesh* debugQuad = nullptr;
	Shader* depthMapDebugShader = nullptr;
	int depthMapDebugTextureUniform = -1;
	glm::mat4 lightSpaceMatrix;
public:
	Renderer(GLFWwindow* mainWindow, glm::ivec2 screenSize);
	~Renderer() {}
	void draw(Scene* scene, UIManager* ui);
	bool isReady() const;
	void setDebugDraw(bool val);
};