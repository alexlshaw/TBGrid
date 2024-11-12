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
		glm::vec3 hitLocation{};
		GameObject* hitTarget = getObjectUnderCursor(hitLocation, Collision::Layer_Unit);
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
	//TODO: Seems to be a bug where spam right-clicking (target spam) can sometimes get the selected unit stuck with an action that never registers as complete
	//we're only going to do our cursor raycast if we actually have a cursor
	if (scene->mainCamera->followCamera && !processingAction && playerTurn)
	{
		//Next check: are we actually targeting anything and have a unit that can act?
		if (currentTarget && currentSelectedUnit && currentSelectedUnit->remainingActionPoints > 0)
		{
			if (currentTarget->name.starts_with("Level Floor"))
			{
				targetFloor(currentTarget);
			}
			else if (currentTarget->name.starts_with("EnemyUnit"))
			{
				targetEnemy(currentTarget);
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
	if (plannedPath.size() > 0 && plannedPath.size() <= currentSelectedUnit->remainingActionPoints)
	{
		std::vector<glm::vec3> spatialPath;
		for (auto& idx : plannedPath)
		{
			spatialPath.push_back(glm::vec3(level->levelGrid.getSpatialCoordsFromCellIndex(idx) + Unit::CELL_OFFSET));
		}
		currentSelectedUnit->assignMovementAction(spatialPath);
		currentSelectedUnit->remainingActionPoints -= static_cast<int>(spatialPath.size());
		processingAction = true;
		ui->updateActionPointUI(currentSelectedUnit, 0);
	}
}

void GameManager::targetEnemy(GameObject* hitTarget)
{
	TurnBoundUnit* enemy = dynamic_cast<TurnBoundUnit*>(hitTarget);
	if (enemy)
	{
		if (currentSelectedUnit->remainingActionPoints >= currentSelectedUnit->attackActionPointCost)
		{
			currentSelectedUnit->assignAttackAction(enemy, scene);
			currentSelectedUnit->remainingActionPoints -= currentSelectedUnit->attackActionPointCost;
			processingAction = true;
			ui->updateActionPointUI(currentSelectedUnit, 0);
		}
	}
	else
	{
		DEBUG_PRINTLN("Object labelled as enemy failed cast to unit in GameManager::targetEnemy()");
	}
}

void GameManager::planActionFromCursor()
{
	//we only plan an action during the player turn
	if (currentSelectedUnit != nullptr && playerTurn)
	{
		glm::vec3 ray = scene->mainCamera->computeRayThroughScreen(Input::mouseCoords());
		glm::vec3 targetLocation = glm::vec3();
		int layerMask = Collision::Layer_LevelGeometry | Collision::Layer_Unit;
		GameObject* hitTarget = getObjectUnderCursor(targetLocation, layerMask);
		if (hitTarget != nullptr && hitTarget != currentTarget)
		{
			currentTarget = hitTarget;
			//The cursor has apparently moved on to a new object this frame (or a player turn has just begun), figure out what the cursor is over
			if (hitTarget->name.starts_with("Level Floor"))
			{
				//we have a selected unit and we're mousing over a location on the floor. Plan a path to get there
				planPath(hitTarget->transform.getPosition() + Unit::CELL_OFFSET);
				if (plannedPath.size() <= currentSelectedUnit->remainingActionPoints)
				{
					ui->updateActionPointUI(currentSelectedUnit, static_cast<int>(plannedPath.size()));
				}
				else
				{
					//oh no, the path is too long
					ui->updateActionPointUI(currentSelectedUnit, 0);
				}
			}
			else if (hitTarget->name.starts_with("EnemyUnit"))
			{
				//we're targeting a unit
				if (currentSelectedUnit->remainingActionPoints >= currentSelectedUnit->attackActionPointCost)
				{
					ui->updateActionPointUI(currentSelectedUnit, currentSelectedUnit->attackActionPointCost);
				}
				else
				{
					//can't attack an enemy if we don't have enough action points
					ui->updateActionPointUI(currentSelectedUnit, 0);
				}
			}
		}
		updatePathIndicator(targetLocation, hitTarget);
	}
}

void GameManager::planPath(glm::vec3 targetLocation)
{
	if (!processingAction)
	{
		plannedPath.clear();
		plannedPath = level->levelGrid.pathBetweenPositions(currentSelectedUnit->transform.getPosition(), targetLocation);
	}
}

void GameManager::switchTurn()
{
	playerTurn = !playerTurn;
	ui->setTurnInfo(playerTurn);
	currentTarget = nullptr;
	updatePathIndicator(glm::vec3{}, nullptr);		//We want the path indicator gone on enemy turns, and immediately visible (if a unit is selected) on player turns
	if (playerTurn)
	{
		std::vector<std::shared_ptr<PlayerUnit>> activePlayers = level->getActivePlayers();
		//It's just switched to the player's turn, update the state of the player unit, and of any relevant UI stuff
		for (auto& player : activePlayers)
		{
			player->resetActionPoints();
		}
		ui->populateUIForSelectedUnit(currentSelectedUnit);
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
				enemy->resetActionPoints();
				enemy->determineAction();
			}
		}
	}
}

void GameManager::processPlayerTurn()
{
	planActionFromCursor();
	//Only allow the target action during the player turn
	if (Input::mouseDown[Input::ACTION_TARGET])
	{
		actionTarget();
	}
	//check if we're in the middle of processing actions for any player units
	std::vector<std::shared_ptr<PlayerUnit>> activePlayers = level->getActivePlayers();
	processingAction = std::any_of(activePlayers.begin(), activePlayers.end(), [](std::shared_ptr<PlayerUnit> player) {return player->hasAction(); });
	if (!processingAction)
	{
		//we're not in the middle of processing an action for any player unit, see if any are waiting for one to be assigned or if it's time to change turn
		if (!std::any_of(activePlayers.begin(), activePlayers.end(), [](std::shared_ptr<PlayerUnit> player) {return player->remainingActionPoints > 0; }))
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
		//in theory, any enemy with remaining action points should be able to choose another action, but we don't do that yet
		if (!std::any_of(activeEnemies.begin(), activeEnemies.end(), [](std::shared_ptr<EnemyUnit> enemy) {return enemy->remainingActionPoints > 0; }))
		{
			switchTurn();
		}
	}
}

void GameManager::updatePathIndicator(glm::vec3 targetLocation, GameObject* targetedObject)
{
	pathIndicator->enabled = false;
	pathCursor->enabled = false;
	if (currentSelectedUnit != nullptr && playerTurn && targetedObject != nullptr)
	{
		if (targetedObject->name.starts_with("Level Floor"))
		{
			if (plannedPath.size() <= currentSelectedUnit->remainingActionPoints)
			{
				//the location is within movement range, make the path elements a nice approving green
				pathIndicator->setColour({ 0.0f, 1.0f, 0.0f, 1.0f });
				pathCursor->getMaterial()->setProperty("albedo", { 0.0f, 1.0f, 0.0f, 1.0f });
			}
			else
			{
				//the location is too far away, make the elements black
				pathIndicator->setColour({ 0.0f, 0.0f, 0.0f, 1.0f });
				pathCursor->getMaterial()->setProperty("albedo", { 0.0f, 0.0f, 0.0f, 1.0f });
			}
			setPathIndicatorLocation(targetLocation, GeometryConstants::CURSOR_DEFAULT_SCALE, GeometryConstants::CURSOR_DEFAULT_OFFSET);
		}
		else if (targetedObject->name.starts_with("EnemyUnit"))
		{
			setPathIndicatorLocation(targetedObject->transform.getPosition(), GeometryConstants::CURSOR_TARGET_SCALE, GeometryConstants::CURSOR_TARGET_OFFSET);
			pathIndicator->setColour({ 1.0f, 0.0f, 0.0f, 1.0f });
			pathCursor->getMaterial()->setProperty("albedo", { 1.0f, 0.0f, 0.0f, 1.0f });
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
	//deselect current unit
	plannedPath.clear();
	if (currentSelectedUnit != nullptr)
	{
		currentSelectedUnit->selectedIndicator->enabled = false;
	}
	//select new unit
	currentSelectedUnit = newSelected;
	if (currentSelectedUnit != nullptr)
	{
		currentSelectedUnit->selectedIndicator->enabled = true;
	}
	//update interface stuff
	updatePathIndicator(glm::vec3{}, nullptr);
	ui->populateUIForSelectedUnit(currentSelectedUnit);
}

GameObject* GameManager::getObjectUnderCursor(glm::vec3& hitLocation, int layerMask)
{
	//compute direction vector from cursor location
	glm::vec3 ray = scene->mainCamera->computeRayThroughScreen(Input::mouseCoords());
	//determine what is under the cursor
	return scene->rayCast(scene->mainCamera->transform.getPosition(), ray, hitLocation, layerMask);
}