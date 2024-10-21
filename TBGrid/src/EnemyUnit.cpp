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
	processAction(deltaTime);
}

void EnemyUnit::determineAction(const LevelGrid& grid)
{
	//for now, we're just going to do a movement of 1 unit up
	std::vector<int> path = grid.pathBetweenPositions(transform.getPosition(), transform.getPosition() + glm::vec3(1.0f, 0.0f, 1.0f));
	if (path.size() > 0)
	{
		std::vector<glm::vec3> spatialPath;
		for (auto& idx : path)
		{
			spatialPath.push_back(glm::vec3(grid.getSpatialCoordsFromCellIndex(idx) + Unit::CELL_OFFSET));
		}
		assignMovementAction(spatialPath);
	}

	//For current testing, just pass the turn. Eventually I'll want to be able to assign an idle action which has them visibly wait for a little bit
	actionAvailable = false;
}
