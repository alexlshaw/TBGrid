#include "Renderer.h"
#include <format>
#include "GameObject.h"
#include "GraphicsResourceManager.h"
#include "glm\gtc\matrix_transform.hpp"

Renderer::Renderer(GLFWwindow* mainWindow, glm::ivec2 screenSize)
	: mainWindow(mainWindow),
	screenSize(screenSize),
	lightSpaceMatrix(glm::identity<mat4>())
{
	readyToDraw = initGL();
	constructDebugObjects();
	initAnimation();
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
			activeMaterial->use(scene->mainCamera, scene->getLights(), lightSpaceMatrix);
			materialActivations++;
			activeShader = activeMaterial->shader;
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
	//pre-activate the shader as but I need it active to set the uniform, and don't want to unneccessarily set the uniform twice since it's big
	drawAnimatedObject(scene->animator, scene);
	if (ui)
	{
		ui->mainCanvas->draw();
	}
	endFrame(scene);
}

void Renderer::beginFrame(Scene* scene)
{
	materialActivations = 0;
	drawCount = 0;
	renderLightingPass(scene);
	glViewport(0, 0, screenSize.x, screenSize.y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene->mainCamera->calculateViewMatrix();
	//bind the shadow map for any materials that use it
	glActiveTexture(GL_TEXTURE_SHADOW_MAP);
	glBindTexture(GL_TEXTURE_2D, depthMap);
}

void Renderer::endFrame(Scene* scene)
{
	//displayDebug();
	DEBUG_PRINT_GL_ERRORS("Renderer.cpp: endFrame()");
	glfwSwapBuffers(mainWindow);
	activeMaterial = nullptr;
}

void Renderer::renderLightingPass(Scene* scene)
{
	//set up for rendering
	//TODO: the following values work well for the current test level. However, they won't work for a level of a different size
	glm::mat4 lightProjection = glm::ortho(-14.0f, 14.0f, -14.0f, 14.0f, 1.0f, 40.0f);
	glm::vec3 lightOffset = { 10.0f, 0.0f, 10.0f };		//adjusted position of light in order to compensate for level not being centered at origin
	glm::mat4 lightView = glm::lookAt(glm::normalize(glm::vec3(-scene->sun->direction)) * 20.0f + lightOffset, lightOffset, { 0.0f, 1.0f, 0.0f });
	lightSpaceMatrix = lightProjection * lightView;
	depthShader->use();
	depthShader->setUniform(depthShaderProjViewUniform, lightSpaceMatrix);
	depthShader->setUniform(depthShaderDiffuseUniform, TEXTURE_DIFFUSE);
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	//render the shadow pass
	for (auto& object : scene->objectsInScene)
	{
		drawObjectLightingPass(object, scene);
	}
	//render the shadow pass for animated objects
	animatedDepthShader->use();
	animatedDepthShader->setUniform(animatedDepthShaderProjViewUniform, lightSpaceMatrix);
	animatedDepthShader->setUniform(animatedDepthShaderDiffuseUniform, TEXTURE_DIFFUSE);
	drawAnimatedObjectLightingPass(scene->animator, scene);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool Renderer::initGL()
{
	//gl stuff
	DEBUG_PRINT("Init GL... ");
	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);	//using LEQUAL so that UI stuff can all share a depth value and just rely on draw order
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
			version = glGetString(GL_VERSION);
			initShadows();
			return true;
		}
	}
	DEBUG_PRINTLN("failed");
	return false;
}

void Renderer::initShadows()
{
	//create frambuffer
	glGenFramebuffers(1, &depthMapFBO);
	//create depth map texture
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColour[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColour);
	//attach depth map to frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);	//Not doing any colour data
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//load the shader
	depthShader = GraphicsResourceManager::getInstance().loadShader("environment/Depth");
	depthShaderProjViewUniform = depthShader->getUniformLocation("projectionViewMatrix");
	depthShaderModelUniform = depthShader->getUniformLocation("modelMatrix");
	depthShaderDiffuseUniform = depthShader->getUniformLocation("diffuseMap");
}

void Renderer::initAnimation()
{
	//stuff for the regular pass of animated objects (most uniforms retrieved in specific material)
	skeletalAnimation = GraphicsResourceManager::getInstance().loadShader("environment/SkeletalAnimation");
	boneMatricesUniform = skeletalAnimation->getUniformLocation("finalBoneMatrices");
	//stuff for the shadows pass of animated objects
	animatedDepthShader = GraphicsResourceManager::getInstance().loadShader("environment/AnimatedDepth");
	animatedDepthShaderProjViewUniform = animatedDepthShader->getUniformLocation("projectionViewMatrix");
	animatedDepthShaderModelUniform = animatedDepthShader->getUniformLocation("modelMatrix");
	animatedDepthShaderDiffuseUniform = animatedDepthShader->getUniformLocation("diffuseMap");
	animatedDepthShaderBoneMatricesUniform = animatedDepthShader->getUniformLocation("finalBoneMatrices");
}

bool Renderer::isReady() const
{
	return readyToDraw;
}

void Renderer::setDebugDraw(bool val)
{
	debugDrawMode = val;
}

void Renderer::drawObject(std::shared_ptr<GameObject> object, Scene* scene)
{
	if (object->enabled && (!object->onlyDrawInDebug || debugDrawMode))
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

void Renderer::drawObjectLightingPass(std::shared_ptr<GameObject> object, Scene* scene)
{
	if (object->enabled)
	{
		int renderPass = -1;
		if (object->castsShadows)
		{
			glm::mat4 model = object->computeEffectiveTransform().getMatrix();
			depthShader->setUniform(depthShaderModelUniform, model);
			object->materials[0]->diffuseMap->use();
			object->draw(renderPass);
		}
		//we iterate the child objects outside the shadow-casting check, because it is entirely possible that a child is a shadow caster while its parent is not 
		// (e.g. child is compound object's visuals)
		for (auto& child : object->children)
		{
			drawObjectLightingPass(child, scene);
		}
	}
}

void Renderer::drawAnimatedObject(Animator* object, Scene* scene)
{
	AnimatedModel* model = object->getModel();
	if (model)
	{
		Transform testTransform({ 1.5f, 0.1f, 1.5f }, glm::identity<mat4>(), glm::vec3(1.0f));
		auto& boneMatrices = object->getFinalBoneMatrices();
		//make sure our animation shader is active
		if (activeShader != skeletalAnimation)
		{
			skeletalAnimation->use();
			activeShader = skeletalAnimation;
		}
		skeletalAnimation->setUniform(boneMatricesUniform, boneMatrices);
		//draw all sub-meshes with their respective materials
		for (size_t i = 0; i < model->meshes.size(); i++)
		{
			setMaterial(model->materials[i], scene);
			activeMaterial->setTransform(testTransform);
			scene->animModel->draw(i);
		}
	}
}

void Renderer::drawAnimatedObjectLightingPass(Animator* object, Scene* scene)
{
	AnimatedModel* model = object->getModel();
	if (model)
	{
		Transform testTransform({ 1.5f, 0.1f, 1.5f }, glm::identity<mat4>(), glm::vec3(1.0f));
		auto& boneMatrices = object->getFinalBoneMatrices();
		if (activeShader != animatedDepthShader)
		{
			animatedDepthShader->use();
			activeShader = animatedDepthShader;
		}
		animatedDepthShader->setUniform(animatedDepthShaderBoneMatricesUniform, boneMatrices);
		// draw all sub - meshes with the diffuse texture of their respective materials
		for (size_t i = 0; i < model->meshes.size(); i++)
		{
			model->materials[i]->diffuseMap->use();
			animatedDepthShader->setUniform(animatedDepthShaderModelUniform, testTransform.getMatrix());
			scene->animModel->draw(i);
		}
	}
}

void Renderer::constructDebugObjects()
{
	std::vector<ColouredVertex> vertices;
	std::vector<unsigned int> indices{ 0, 1, 2, 0, 2, 3 };

	//Quad is rendered without transformation, so define vertices in NDC
	ColouredVertex v1{}, v2{}, v3{}, v4{};
	v1.position = glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);	v1.normal = glm::vec3(0.0f, 0.0f, 1.0f);	v1.texCoords = glm::vec2(0.0f, 0.0f);	v1.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	v2.position = glm::vec4(0.0f, 1.0f, -1.0f, 1.0f);	v2.normal = glm::vec3(0.0f, 0.0f, 1.0f);	v2.texCoords = glm::vec2(0.0f, 1.0f);	v2.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	v3.position = glm::vec4(1.0f, 1.0f, -1.0f, 1.0f);	v3.normal = glm::vec3(0.0f, 0.0f, 1.0f);	v3.texCoords = glm::vec2(1.0f, 1.0f);	v3.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	v4.position = glm::vec4(1.0f, 0.0f, -1.0f, 1.0f);	v4.normal = glm::vec3(0.0f, 0.0f, 1.0f);	v4.texCoords = glm::vec2(1.0f, 0.0f);	v4.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v4);

	debugQuad = new Mesh("DebugQuad", vertices, indices);
	GraphicsResourceManager& resourceManager = GraphicsResourceManager::getInstance();
	resourceManager.addMesh("DebugQuad", debugQuad);
	depthMapDebugShader = resourceManager.loadShader("environment/DepthMap");
	depthMapDebugTextureUniform = depthMapDebugShader->getUniformLocation("diffuseMap");
}

void Renderer::displayDebug()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	depthMapDebugShader->use();
	depthMapDebugShader->setUniform(depthMapDebugTextureUniform, 0);
	debugQuad->draw();
}
