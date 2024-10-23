#include "GameManager.h"

GameManager::GameManager(Scene* mainScene, Level* currentLevel, UIManager* ui)
	:	scene(mainScene),
		level(currentLevel),
		ui(ui)
{
	pathIndicator = static_cast<LineRenderer*>(scene->findObjectByName("Path Indicator"));
	pathCursor = static_cast<StaticMesh*>(scene->findObjectByName("Path Cursor"));
	pathIndicator->enabled = false;
	pathCursor->enabled = false;
	if (pathIndicator == nullptr || pathCursor == nullptr)
	{
		DEBUG_PRINTLN("GameManager failed to find core scene object(s)");
	}
	//Find players and enemies
	activePlayer = static_cast<PlayerUnit*>(scene->findObjectByName("PlayerUnit"));
	activeEnemy = static_cast<EnemyUnit*>(scene->findObjectByName("EnemyUnit"));
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
		if (hitTarget != nullptr && hitTarget->name == "PlayerUnit")
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
		if (hitTarget != nullptr && hitTarget->name == "Level Floor")
		{
			if (currentSelectedUnit != nullptr)
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

void GameManager::switchTurn()
{
	playerTurn = !playerTurn;
	ui->setTurnInfo(playerTurn);
	updatePathIndicator();		//We want the path indicator gone on enemy turns, and immediately visible (if a unit is selected) on player turns
	if (playerTurn)
	{
		//It's just switched to the player's turn, update the state of the player unit, and of any relevant UI stuff
		if (activePlayer)
		{
			activePlayer->actionAvailable = true;
		}
	}
	else
	{
		//It's just switched to the enemy turn, do AI stuff
		if (activeEnemy)
		{
			activeEnemy->actionAvailable = true;
			activeEnemy->determineAction(level->levelGrid);
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
	processingAction = activePlayer->hasAction();
	if (!processingAction)
	{
		//we're not in the middle of processing an action for the player unit, see if we should wait for it to be assigned one or flip the turn
		if (!activePlayer->actionAvailable)
		{
			switchTurn();
		}
	}
}

void GameManager::processEnemyTurn()
{
	//check if the enemy unit has finished doing its thing
	processingAction = activeEnemy->hasAction();
	if (!processingAction)
	{
		//enemy is assigned actions on turn change, so if they're not processing one it should be time to switch turn
		if (!activeEnemy->actionAvailable)	//Which means that we shouldn't actually need this check, or the actionAvailable variable for enemies in general (but will keep for actionPoint change)
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
		GameObject* hitTarget = scene->rayCast(scene->mainCamera->transform.getPosition(), ray, targetLocation);
		if (hitTarget != nullptr && hitTarget->name == "Level Floor")
		{
			//update the path line
			glm::vec3 pathOffset(0.0f, 0.01f, 0.0f);	//Raise the indicator slightly to avoid clipping
			std::vector<glm::vec3> points;
			points.push_back(currentSelectedUnit->transform.getPosition() + pathOffset);
			points.push_back(targetLocation + pathOffset);
			pathIndicator->generateSegmentsFromPoints(points);
			pathIndicator->enabled = true;
			//update the end of path marker
			pathCursor->transform.setPosition(targetLocation + glm::vec3(-0.2f, 0.11f, -0.2f));
			pathCursor->enabled = true;
		}
	}
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
	GameObject* hitTarget = scene->rayCast(scene->mainCamera->transform.getPosition(), ray, hitLocation);
	//Announce whatever we have clicked on
	//std::cout << std::format("Hit {} at: ({}, {}, {})\n", hitTarget->name, hitLocation.x, hitLocation.y, hitLocation.z);
	return hitTarget;
}