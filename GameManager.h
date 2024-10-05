#pragma once

#include "glm/glm.hpp"
#include "DebuggingTools.h"
#include "Level.h"
#include "LineRenderer.h"
#include "Scene.h"
#include "PlayerUnit.h"

#include "Input.h"

//This class is responsible for handling the ongoing state of the game while a level is loaded
class GameManager
{
private:
	Scene* scene;
	Level* level;
	PlayerUnit* currentSelectedUnit = nullptr;
	LineRenderer* pathIndicator = nullptr;
	StaticMesh* pathCursor = nullptr;
	bool processingAction = false;	//Whether to block interaction while waiting for an action to finish
	void actionSelect();
	void actionTarget();
	void selectUnit(PlayerUnit* newSelected);
	GameObject* getObjectUnderCursor();
	void updatePathIndicator();
public:
	GameManager(Scene* mainScene, Level* level);
	void update(float deltaTime);
	void init();	//Initialisation code to be run after the scene is loaded
};