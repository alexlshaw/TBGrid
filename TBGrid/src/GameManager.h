#pragma once

#include "glm/glm.hpp"
#include "DebuggingTools.h"
#include "EnemyUnit.h"
#include "Level.h"
#include "LineRenderer.h"
#include "Scene.h"
#include "PlayerUnit.h"
#include "UI/UIManager.h"

#include "Input.h"

//This class is responsible for handling the ongoing state of the game while a level is loaded
class GameManager
{
private:
	Scene* scene;
	Level* level;
	UIManager* ui;
	PlayerUnit* activePlayer = nullptr;			//The player unit on the field (to be replaced by a vector of them, probably)
	EnemyUnit* activeEnemy = nullptr;			//The enemy unit on the field (to be replaced by a vector of them, probably)
	PlayerUnit* currentSelectedUnit = nullptr;
	LineRenderer* pathIndicator = nullptr;
	StaticMesh* pathCursor = nullptr;
	bool processingAction = false;	//Whether to block interaction while waiting for an action to finish
	void selectUnit(PlayerUnit* newSelected);
	GameObject* getObjectUnderCursor();
	void updatePathIndicator();
	void setPathIndicatorLocation(glm::vec3 location, glm::vec3 cursorScale);
	void processPlayerTurn();
	void processEnemyTurn();
	//Action functions
	void actionSelect();
	void actionTarget();
	void actionFocus();
	
public:
	GameManager(Scene* mainScene, Level* level, UIManager* ui);
	void update(float deltaTime);
	void switchTurn();
	bool playerTurn = true;
};