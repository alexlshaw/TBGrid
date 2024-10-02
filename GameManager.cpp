#include "GameManager.h"

GameManager::GameManager(Scene* mainScene)
	:	scene(mainScene)
{

}

void GameManager::update(float deltaTime)
{
	if (Input::mouseDown[Input::ACTION_SELECT])
	{
		actionSelect();
	}
	if (Input::mouseDown[Input::ACTION_TARGET])
	{
		actionTarget();
	}
	updatePathIndicator();
	if (processingAction && currentSelectedUnit != nullptr)
	{
		processingAction = currentSelectedUnit->hasAction;
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
	if (scene->mainCamera->followCamera && !processingAction)
	{
		//determine what is under the cursor
		GameObject* hitTarget = getObjectUnderCursor();
		if (hitTarget != nullptr && hitTarget->name == "Level Tile")
		{
			if (currentSelectedUnit != nullptr)
			{
				currentSelectedUnit->assignMovementAction(hitTarget->transform.getPosition() + glm::vec3(0.5f, 0.1f, 0.5f));
				processingAction = true;
			}
		}
	}
}

void GameManager::updatePathIndicator()
{
	pathIndicator->enabled = false;
	pathCursor->enabled = false;
	if (currentSelectedUnit != nullptr)
	{
		glm::vec3 ray = scene->mainCamera->computeRayThroughScreen(Input::mouseCoords());
		glm::vec3 targetLocation = glm::vec3();
		GameObject* hitTarget = scene->rayCast(scene->mainCamera->transform.getPosition(), ray, targetLocation);
		if (hitTarget != nullptr && hitTarget->name == "Level Tile")
		{
			//update the path line
			glm::vec3 pathOffset(0.0f, 0.01f, 0.0f);	//Raise the indicator slightly to avoid clipping
			std::vector<glm::vec3> points;
			points.push_back(currentSelectedUnit->transform.getPosition() + pathOffset);
			points.push_back(targetLocation + pathOffset);
			pathIndicator->generateSegmentsFromPoints(points);
			pathIndicator->enabled = true;
			//update the end of path marker
			pathCursor->transform.setPosition(targetLocation + glm::vec3(-0.2f, 0.01f, -0.2f));
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

void GameManager::init()
{
	pathIndicator = static_cast<LineRenderer*>(scene->findObjectByName("Path Indicator"));
	pathCursor = static_cast<StaticMesh*>(scene->findObjectByName("Path Cursor"));
	if (pathIndicator == nullptr || pathCursor == nullptr)
	{
		DEBUG_PRINTLN("GameManager failed to find core scene object(s)");
	}
}