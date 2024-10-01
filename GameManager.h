#pragma once

#include "glm/glm.hpp"
#include "DebuggingTools.h"
#include "Scene.h"
#include "PlayerUnit.h"

#include "Input.h"

//This class is responsible for handling the ongoing state of the game while a level is loaded
class GameManager
{
private:
	Scene* scene;
	PlayerUnit* currentSelectedUnit = nullptr;
	void selectUnit(PlayerUnit* newSelected);
public:
	GameManager(Scene* mainScene);
	void update(float deltaTime);
};