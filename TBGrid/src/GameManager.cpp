#include "GameManager.h"
#include <algorithm>

namespace GeometryConstants
{
	glm::vec3 CURSOR_DEFAULT_SCALE = glm::vec3(0.4f, 1.0f, 0.4f);
	glm::vec3 CURSOR_TARGET_SCALE = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 PATH_START_OFFSET = glm::vec3(0.0f, 0.05f, 0.0f);	//Raising slightly to avoid clipping
	glm::vec3 CURSOR_DEFAULT_OFFSET = glm::vec3(-0.20f, 0.055f, -0.20f);	//Raising slightly to avoid clipping
	glm::vec3 CURSOR_TARGET_OFFSET = glm::vec3(-0.5f, 0.055f, -0.5f);	//Raising slightly to avoid clipping
}

GameManager::GameManager(Scene* mainScene, Level* currentLevel, UIManager* ui)
	:	scene(mainScene),
		level(currentLevel),
		ui(ui)
{
	//find path stuff
	pathIndicator = static_cast<LineRenderer*>(scene->findObjectByName("Path Indicator"));
	pathCursor = static_cast<StaticMesh*>(scene->findObjectByName("Path Cursor"));
	if (!pathIndicator || !pathCursor)
	{
		DEBUG_PRINTLN("GameManager failed to find core scene object(s)");
	}
	else
	{
		pathIndicator->enabled = false;
		pathCursor->enabled = false;
	}
}

void GameManager::update(float deltaTime)
{
	if (Input::mouseDown[Input::ACTION_SELECT])
	{
		//ACTION_SELECT is a click, so lets see if the UI wants to handle it first
		if (!ui->mainCanvas->onClick(Input::mouseCoords()))
		{
			actionSelect();
		}
	}
	if (Input::getKeyDown(Input::ACTION_FOCUS))
	{
		actionFocus();
	}

	if (playerTurn)
	{
		processPlayerTurn();
	}
	else
	{
		processEnemyTurn();
	}
}

void GameManager::actionSelect()
{
	//we're only going to do our cursor raycast if we actually have a cursor
	if (scene->mainCamera->followCamera && !processingAction)
	{
		//We want both the object and location here
		GameObject* hitTarget = getObjectUnderCursor();
		if (hitTarget != nullptr && hitTarget->name.starts_with("PlayerUnit"))
		{
			selectUnit(static_cast<PlayerUnit*>(hitTarget));
		}
		else
		{
			selectUnit(nullptr);
		}
	}
}

void GameManager::actionTarget()
{
	//we're only going to do our cursor raycast if we actually have a cursor
	if (scene->mainCamera->followCamera && !processingAction && playerTurn)
	{
		//determine what is under the cursor
		GameObject* hitTarget = getObjectUnderCursor();
		if (hitTarget && currentSelectedUnit && currentSelectedUnit->actionAvailable)
		{
			if (hitTarget->name.starts_with("Level Floor"))
			{
				targetFloor(hitTarget);
			}
			else if (hitTarget->name.starts_with("EnemyUnit"))
			{
				targetEnemy(hitTarget);
			}
		}
	}
}

void GameManager::actionFocus()
{
	//updates the view to focus on the current selected unit
	if (currentSelectedUnit != nullptr)
	{
		scene->mainCamera->switchToFollowMode();
		scene->mainCamera->followTarget = currentSelectedUnit->transform.getPosition();
		scene->mainCamera->updateFollowingPosition();
	}
}

void GameManager::targetFloor(GameObject* hitTarget)
{
	glm::vec3 movementTarget = hitTarget->transform.getPosition() + Unit::CELL_OFFSET;
	std::vector<int> path = level->levelGrid.pathBetweenPositions(currentSelectedUnit->transform.getPosition(), movementTarget);
	if (path.size() > 0)
	{
		std::vector<glm::vec3> spatialPath;
		for (auto& idx : path)
		{
			spatialPath.push_back(glm::vec3(level->levelGrid.getSpatialCoordsFromCellIndex(idx) + Unit::CELL_OFFSET));
		}

		currentSelectedUnit->assignMovementAction(spatialPath, level->levelGrid);
		currentSelectedUnit->actionAvailable = false;
		processingAction = true;
	}
}

void GameManager::targetEnemy(GameObject* hitTarget)
{
	TurnBoundUnit* enemy = dynamic_cast<TurnBoundUnit*>(hitTarget);
	if (enemy)
	{
		currentSelectedUnit->assignAttackAction(enemy, scene);
		currentSelectedUnit->actionAvailable = false;
		processingAction = true;
	}
	else
	{
		DEBUG_PRINTLN("Object labelled as enemy failed cast to unit in GameManager::targetEnemy()");
	}
}

void GameManager::switchTurn()
{
	playerTurn = !playerTurn;
	ui->setTurnInfo(playerTurn);
	updatePathIndicator();		//We want the path indicator gone on enemy turns, and immediately visible (if a unit is selected) on player turns
	if (playerTurn)
	{
		std::vector<std::shared_ptr<PlayerUnit>> activePlayers = level->getActivePlayers();
		//It's just switched to the player's turn, update the state of the player unit, and of any relevant UI stuff
		for (auto& player : activePlayers)
		{
			player->actionAvailable = true;
		}
	}
	else
	{
		//It's just switched to the enemy turn, process all enemies
		std::vector<std::shared_ptr<EnemyUnit>> activeEnemies = level->getActiveEnemies();
		if (activeEnemies.size() == 0)
		{
			//No enemies present, immediately return to player turn
			switchTurn();
		}
		else
		{
			//enemies present, do their AI
			for (auto& enemy : activeEnemies)
			{
				enemy->actionAvailable = true;
				enemy->determineAction(level->levelGrid);
			}
		}
	}
}

void GameManager::processPlayerTurn()
{
	//Only allow the target action during the player turn
	if (Input::mouseDown[Input::ACTION_TARGET])
	{
		actionTarget();
	}
	//check if we need to wait for the player unit to finish doing its thing
	updatePathIndicator();
	std::vector<std::shared_ptr<PlayerUnit>> activePlayers = level->getActivePlayers();
	//check if we're in the middle of processing actions for any player units
	processingAction = std::any_of(activePlayers.begin(), activePlayers.end(), [](std::shared_ptr<PlayerUnit> player) {return player->hasAction(); });
	if (!processingAction)
	{
		//we're not in the middle of processing an action for any player unit, see if any are waiting for one to be assigned or if it's time to change turn
		if (!std::any_of(activePlayers.begin(), activePlayers.end(), [](std::shared_ptr<PlayerUnit> player) {return player->actionAvailable; }))
		{
			switchTurn();
		}
	}
}

void GameManager::processEnemyTurn()
{
	//check if the enemy unit has finished doing its thing
	std::vector<std::shared_ptr<EnemyUnit>> activeEnemies = level->getActiveEnemies();
	//check if we're in the middle of processing actions for any enemies
	processingAction = std::any_of(activeEnemies.begin(), activeEnemies.end(), [](std::shared_ptr<EnemyUnit> enemy) {return enemy->hasAction(); });
	if (!processingAction)
	{
		//enemy is assigned actions on turn change, so if they're not processing one it should be time to switch turn
		//Which means that we shouldn't actually need this check, or the actionAvailable variable for enemies in general (but will keep for actionPoint change)
		if (!std::any_of(activeEnemies.begin(), activeEnemies.end(), [](std::shared_ptr<EnemyUnit> enemy) {return enemy->actionAvailable; }))
		{
			switchTurn();
		}
	}
}

void GameManager::updatePathIndicator()
{
	pathIndicator->enabled = false;
	pathCursor->enabled = false;
	if (currentSelectedUnit != nullptr && playerTurn)
	{
		glm::vec3 ray = scene->mainCamera->computeRayThroughScreen(Input::mouseCoords());
		glm::vec3 targetLocation = glm::vec3();
		int layerMask = Collision::Layer_LevelGeometry | Collision::Layer_Unit;
		GameObject* hitTarget = scene->rayCast(scene->mainCamera->transform.getPosition(), ray, targetLocation, layerMask);
		if (hitTarget != nullptr)
		{
			//TODO: Life will get a lot easier if my targeting indicators a circles, because then I'll just be able to pass in the radius to figure out the path extent
			if (hitTarget->name.starts_with("Level Floor"))
			{
				setPathIndicatorLocation(targetLocation, GeometryConstants::CURSOR_DEFAULT_SCALE, GeometryConstants::CURSOR_DEFAULT_OFFSET);
				pathIndicator->setColour({ 0.0f, 1.0f, 0.0f, 1.0f });
				pathCursor->getMaterial()->setProperty("albedo", { 0.0f, 1.0f, 0.0f, 1.0f });
			}
			else if (hitTarget->name.starts_with("EnemyUnit"))
			{
				setPathIndicatorLocation(hitTarget->transform.getPosition(), GeometryConstants::CURSOR_TARGET_SCALE, GeometryConstants::CURSOR_TARGET_OFFSET);
				pathIndicator->setColour({ 1.0f, 0.0f, 0.0f, 1.0f });
				pathCursor->getMaterial()->setProperty("albedo", { 1.0f, 0.0f, 0.0f, 1.0f });
			}
		}
	}
}

void GameManager::setPathIndicatorLocation(glm::vec3 location, glm::vec3 cursorScale, glm::vec3 cursorOffset)
{
	//location is the target for the end of the path
	std::vector<glm::vec3> points;
	points.push_back(currentSelectedUnit->transform.getPosition() + GeometryConstants::PATH_START_OFFSET);
	points.push_back(location);
	pathIndicator->generateSegmentsFromPoints(points);
	pathIndicator->enabled = true;
	//update the end of path marker
	pathCursor->transform.setPosition(location + cursorOffset);
	pathCursor->transform.setScale(cursorScale);
	pathCursor->enabled = true;
}

void GameManager::selectUnit(PlayerUnit* newSelected)
{
	if (currentSelectedUnit != nullptr)
	{
		currentSelectedUnit->selectedIndicator->enabled = false;
	}
	currentSelectedUnit = newSelected;
	if (currentSelectedUnit != nullptr)
	{
		currentSelectedUnit->selectedIndicator->enabled = true;
	}
	updatePathIndicator();
}

GameObject* GameManager::getObjectUnderCursor()
{
	glm::vec3 hitLocation(0.0f, 0.0f, 0.0f);
	//compute direction vector from cursor location
	glm::vec3 ray = scene->mainCamera->computeRayThroughScreen(Input::mouseCoords());
	//determine what is under the cursor
	GameObject* hitTarget = scene->rayCast(scene->mainCamera->transform.getPosition(), ray, hitLocation, Collision::Layer_All);
	//Announce whatever we have clicked on
	//std::cout << std::format("Hit {} at: ({}, {}, {})\n", hitTarget->name, hitLocation.x, hitLocation.y, hitLocation.z);
	return hitTarget;
}