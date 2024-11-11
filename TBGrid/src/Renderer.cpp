#include "Renderer.h"
#include "GameObject.h"

Renderer::Renderer(GLFWwindow* mainWindow)
	: mainWindow(mainWindow)
{
	readyToDraw = initGL();
	version = glGetString(GL_VERSION);
}

void Renderer::setMaterial(Material* material, Scene* scene)
{
	if (!material)
	{
		setMaterial(GraphicsResourceManager::getInstance().defaultMaterial, scene);
	}
	else
	{
		if (material != activeMaterial)
		{
			activeMaterial = material;
			activeMaterial->use(scene->mainCamera, scene->lights[0]);
			materialActivations++;
		}
	}
}

void Renderer::draw(Scene* scene, UIManager* ui)
{
	beginFrame(scene);
	for (auto& object : scene->objectsInScene)
	{
		drawObject(object, scene);
	}
	ui->mainCanvas->draw();
	endFrame(scene);
}

void Renderer::beginFrame(Scene* scene)
{
	materialActivations = 0;
	drawCount = 0;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene->mainCamera->calculateViewMatrix();
}

void Renderer::endFrame(Scene* scene)
{
	DEBUG_PRINT_GL_ERRORS("Renderer.cpp: endFrame()");
	glfwSwapBuffers(mainWindow);
	activeMaterial = nullptr;
}

bool Renderer::initGL()
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
	if (glGetError() == GL_NO_ERROR)
	{
		glActiveTexture(GL_TEXTURE0);
		DEBUG_PRINTLN("finished");
		//check the framebuffer, if it's no good we drop out the the final return false
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		{
			return true;
		}
	}
	DEBUG_PRINTLN("failed");
	return false;
}

bool Renderer::isReady() const
{
	return readyToDraw;
}

void Renderer::drawObject(std::shared_ptr<GameObject> object, Scene* scene)
{
	if (object->enabled)
	{
		int passes = object->getRenderPasses();
		for (int pass = 0; pass < passes; pass++)
		{
			setMaterial(object->materials[pass], scene);
			object->draw(pass);
			drawCount++;
		}
		for (auto& child : object->children)
		{
			drawObject(child, scene);
		}
	}
}