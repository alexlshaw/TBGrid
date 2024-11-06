#include "EnemyUnit.h"

EnemyUnit::EnemyUnit()
{
	GraphicsResourceManager& resourceManager = GraphicsResourceManager::getInstance();
	name = "EnemyUnit";
	//Probably need to place a default material here
	Mesh* cube = resourceManager.loadMesh("unit_cube");
	Material* defaultRed = resourceManager.loadMaterial("DefaultGrey");
	collider = std::make_unique<BoxCollider>();
	collider->offset = glm::vec3(-0.5f, 0.0f, -0.5f);
	collider->layer = Collision::Layer_Unit;

	std::shared_ptr<StaticMesh> pillar = std::make_shared<StaticMesh>(cube, defaultRed);
	pillar->name = "Enemy Unit pillar";
	pillar->transform = Transform(glm::vec3(-0.25f, 0.0f, -0.25f), glm::identity<glm::mat4>(), glm::vec3(0.5f, 0.8f, 0.5f));
	addChild(pillar);

	std::shared_ptr<StaticMesh> flatBase = std::make_shared<StaticMesh>(cube, defaultRed);
	flatBase->name = "Enemy Unit base";
	flatBase->transform = Transform(glm::vec3(-0.4f, 0.0f, -0.4f), glm::identity<glm::mat4>(), glm::vec3(0.8f, 0.1f, 0.8f));
	addChild(flatBase);
}

void EnemyUnit::update(const float deltaTime)
{
	//for now, we only need to process the action, but that will change in the future
	if (processAction(deltaTime))
	{
		//we have finished the action, time to remove it
		action.reset();
		action = nullptr;
	}
}

void EnemyUnit::determineAction(LevelGrid& grid)
{
	//truncate spatial coordinates to get unit coordinates
	glm::vec3 position = transform.getPosition();
	glm::ivec3 currentUnitCoords(static_cast<int>(position.x), static_cast<int>(position.y), static_cast<int>(position.z));
	//determine unit coordinates for a target location
	std::vector<std::pair<int, int>> directions = { {-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };
	glm::ivec3 targetCoords = currentUnitCoords;
	bool valid = false;
	int attempts = 0;
	int targetDirectionIdx = rand() % 8;
	while (!valid && attempts < 8)
	{
		targetCoords.x = currentUnitCoords.x + directions[targetDirectionIdx].first;
		targetCoords.z = currentUnitCoords.z + directions[targetDirectionIdx].second;
		valid = grid.validCell(targetCoords) && grid.pathableCell(targetCoords);
		attempts++;
		targetDirectionIdx = (targetDirectionIdx + 1) % 8;
	}
	if (valid)
	{
		//we found a spot to go to
		std::vector<int> path = grid.pathBetweenTwoCells(grid.getCellIndexFromUnitCoords(currentUnitCoords), grid.getCellIndexFromUnitCoords(targetCoords));
		if (path.size() > 0)
		{
			std::vector<glm::vec3> spatialPath;
			for (auto& idx : path)
			{
				spatialPath.push_back(glm::vec3(grid.getSpatialCoordsFromCellIndex(idx) + Unit::CELL_OFFSET));
			}
			assignMovementAction(spatialPath, grid);
		}
	}
	else
	{
		//we did not find a spot to go to
		assignIdleAction();
	}
	//they've been given an action, they're no longer available to recieve one
	actionAvailable = false;
}
