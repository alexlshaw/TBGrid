#include "EnemyUnit.h"
#include "Vertex.h"



EnemyUnit::EnemyUnit(LevelGrid* grid)
	: TurnBoundUnit(grid)
{
	GraphicsResourceManager& resourceManager = GraphicsResourceManager::getInstance();
	name = "EnemyUnit";
	//Probably need to place a default material here
	Mesh* cube = resourceManager.loadMesh("unit_cube");
	Material* defaultEnemy = resourceManager.loadMaterial("DefaultEnemy");
	collider = std::make_unique<BoxCollider>();
	collider->offset = glm::vec3(-0.5f, 0.0f, -0.5f);
	collider->layer = Collision::Layer_Unit;


	Mesh* flatMesh = nullptr;
	if (resourceManager.hasMesh("BasicEnemy"))
	{
		flatMesh = resourceManager.loadMesh("BasicEnemy");
	}
	else
	{
		flatMesh = constructFlatMesh();
		
	}

	std::shared_ptr<StaticMesh> visuals = std::make_shared<StaticMesh>(flatMesh, defaultEnemy);
	visuals->name = "Enemy Visuals";
	visuals->transform = Transform(glm::vec3(-0.5f, 0.0f, 0.0f), glm::identity<mat4>(), glm::vec3(1.0f, 1.0f, 1.0f));
	addChild(visuals);
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

Mesh* EnemyUnit::constructFlatMesh()
{
	//build a simple mesh to show the enemy unit
	std::vector<ColouredVertex> vertices;
	std::vector<unsigned int> indices{ 0, 1, 2, 0, 2, 3 };

	ColouredVertex v1, v2, v3, v4;
	v1.position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);	v1.normal = glm::vec3(0.0f, 0.0f, 1.0f);	v1.texCoords = glm::vec2(0.0f, 0.0f);	v1.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	v2.position = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);	v2.normal = glm::vec3(0.0f, 0.0f, 1.0f);	v2.texCoords = glm::vec2(0.0f, 1.0f);	v2.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	v3.position = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);	v3.normal = glm::vec3(0.0f, 0.0f, 1.0f);	v3.texCoords = glm::vec2(1.0f, 1.0f);	v3.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	v4.position = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);	v4.normal = glm::vec3(0.0f, 1.0f, 0.0f);	v4.texCoords = glm::vec2(1.0f, 0.0f);	v4.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v4);

	Mesh* enemyMesh = new Mesh("BasicEnemy", vertices, indices);
	GraphicsResourceManager::getInstance().addMesh("BasicEnemy", enemyMesh);
	return enemyMesh;
}

void EnemyUnit::determineAction()
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
		valid = levelGrid->validCell(targetCoords) && levelGrid->pathableCell(targetCoords);
		attempts++;
		targetDirectionIdx = (targetDirectionIdx + 1) % 8;
	}
	if (valid)
	{
		//we found a spot to go to
		std::vector<int> path = levelGrid->pathBetweenTwoCells(levelGrid->getCellIndexFromUnitCoords(currentUnitCoords), levelGrid->getCellIndexFromUnitCoords(targetCoords));
		if (path.size() > 0)
		{
			std::vector<glm::vec3> spatialPath;
			for (auto& idx : path)
			{
				spatialPath.push_back(glm::vec3(levelGrid->getSpatialCoordsFromCellIndex(idx) + Unit::CELL_OFFSET));
			}
			assignMovementAction(spatialPath);
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
