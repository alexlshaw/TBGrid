#pragma once

#include <vector>
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
	PlayerUnit* currentSelectedUnit = nullptr;
	LineRenderer* pathIndicator = nullptr;
	StaticMesh* pathCursor = nullptr;
	bool processingAction = false;	//Whether to block interaction while waiting for an action to finish
	void selectUnit(PlayerUnit* newSelected);
	GameObject* getObjectUnderCursor(glm::vec3& hitLocation, int layerMask = Collision::Layer_All);
	void updatePathIndicator(glm::vec3 targetLocation, GameObject* targetedObject);
	void setPathIndicatorLocation(glm::vec3 location, glm::vec3 cursorScale, glm::vec3 cursorOffset);
	void processPlayerTurn();
	void processEnemyTurn();
	//Action functions
	void actionSelect();
	void actionTarget();
	void actionFocus();
	void checkAbilityActivation();

	void targetFloor(GameObject* hitTarget);
	void targetEnemy(GameObject* hitTarget);
	void planActionFromCursor();
	void planPath(glm::vec3 targetLocation);
	std::vector<int> plannedPath;
	GameObject* currentTarget = nullptr;	//Whatever object the cursor is hovering over. We track this to save on pathfinding calculations when the cursor hasn't moved between frames
public:
	GameManager(Scene* mainScene, Level* level, UIManager* ui);
	void update(float deltaTime);
	void switchTurn();
	bool playerTurn = true;
};