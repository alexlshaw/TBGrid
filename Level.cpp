#include "Level.h"

Level::Level(GraphicsResourceManager* resourceManager) 
	: resourceManager(resourceManager) {}

Level::~Level() 
{
	//We trust the shared_ptr to manage any deletions, so no action required here
}

void Level::buildTestLevel()
{
	buildCoreObjects();
	//Add lights
	lights.push_back(Light(glm::vec3(0.25f, 0.25f, 0.25f), glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(), glm::vec3(-1000.0f, 1000.0f, -1000.0f)));
	//Add objects
	Mesh* cube;
	Material* defaultMaterial;

	defaultMaterial = resourceManager->loadMaterial("DefaultLit");
	cube = resourceManager->loadMesh("unit_cube");
	for (int x = 0; x < 20; x++)
	{
		for (int z = 0; z < 20; z++)
		{
			std::shared_ptr<StaticMesh> levelBaseTile = std::make_shared<StaticMesh>(cube, defaultMaterial);
			levelBaseTile->name = "Level Tile";
			levelBaseTile->collider = std::make_unique<BoxCollider>();
			levelBaseTile->transform = Transform(glm::vec3((float)x, -0.1f, float(z)), glm::identity<glm::mat4>(), glm::vec3(1.0f, 0.1f, 1.0f));
			objects.push_back(levelBaseTile);
			
		}
	}

	//Create a unit for the player to control
	std::shared_ptr<PlayerUnit> playerUnit = std::make_shared<PlayerUnit>(resourceManager);
	playerUnit->transform = Transform(glm::vec3(0.5f, 0.0f, 0.5f), glm::identity<glm::mat4>(), glm::vec3(1.0f, 1.0f, 1.0f));
	objects.push_back(playerUnit);

	
}

void Level::loadLevel(std::string levelName)
{
	DEBUG_PRINT("Error: Loading level from file is not yet implemented\n");
}

void Level::buildCoreObjects()
{
	//Create a line renderer for unit paths
	std::shared_ptr<LineRenderer> pathIndicator = std::make_shared<LineRenderer>(resourceManager, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	std::vector<glm::vec3> linePoints = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, -1.0f)
	};
	pathIndicator->generateSegmentsFromPoints(linePoints);
	pathIndicator->name = "Path Indicator";
	objects.push_back(pathIndicator);

	//Create a planar cursor
	std::shared_ptr pathCursor = std::make_shared<StaticMesh>(resourceManager->loadMesh("unit_plane"), resourceManager->loadMaterial("SelectionIndicator"));
	pathCursor->name = "Path Cursor";
	pathCursor->transform.setPosition(glm::vec3(- 0.2f, 0.01f, -0.2f));
	pathCursor->transform.setScale(glm::vec3(0.4f, 1.0f, 0.4f));
	pathCursor->enabled = false;
	objects.push_back(pathCursor);
}