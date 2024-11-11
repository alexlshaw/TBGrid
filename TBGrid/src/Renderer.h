#pragma once

#include "GraphicsResourceManager.h"	//Implicitly includes all graphics objects
#include "Scene.h"
#include "UI/UIManager.h"
#include "glm/glm.hpp"
#include <memory>

#include "glad/gl.h"
#include "GLFW/glfw3.h"

class Renderer
{
private:
	const GLubyte* version;
	bool readyToDraw;
	GLFWwindow* mainWindow;
	Material* activeMaterial = nullptr;
	int materialActivations = 0;
	int drawCount = 0;
	void setMaterial(Material* material, Scene* scene);
	void beginFrame(Scene* scene);
	void endFrame(Scene* scene);
	bool initGL();
	void drawObject(std::shared_ptr<GameObject> object, Scene* scene);
public:
	Renderer(GLFWwindow* mainWindow);
	~Renderer() {}
	void draw(Scene* scene, UIManager* ui);
	bool isReady() const;
};