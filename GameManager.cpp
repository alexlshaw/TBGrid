#include "GameManager.h"

GameManager::GameManager(Scene* mainScene)
	:	scene(mainScene)
{

}

void GameManager::update(float deltaTime)
{
	if (Input::mouseDown_Left)
	{
		//we're only going to do our cursor raycast if we actually have a cursor
		if (scene->mainCamera->followCamera)
		{
			glm::vec3 hitLocation(0.0f, 0.0f, 0.0f);
			//compute direction vector from cursor location
			glm::vec3 ray = scene->mainCamera->computeRayThroughScreen(glm::vec2(static_cast<float>(Input::mouseX), static_cast<float>(Input::mouseY)));
			//determine what is under the cursor
			GameObject* hitTarget = scene->rayCast(scene->mainCamera->transform.getPosition(), ray, hitLocation);
			if (hitTarget != nullptr)
			{
				//Announce whatever we have clicked on
				//std::cout << std::format("Hit {} at: ({}, {}, {})\n", hitTarget->name, hitLocation.x, hitLocation.y, hitLocation.z);
				if (hitTarget->name == "PlayerUnit")
				{
					selectUnit(static_cast<PlayerUnit*>(hitTarget));
				}
				else
				{
					selectUnit(nullptr);
				}
			}
			else
			{
				selectUnit(nullptr);
			}
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