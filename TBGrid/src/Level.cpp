#include "Level.h"
#include "Billboard.h"
#include "BoxCollider.h"
#include "DebuggingTools.h"
#include "GraphicsResourceManager.h"
#include "LightMarker.h"
#include "LineRenderer.h"
#include "MeshTools.h"
#include "StaticMesh.h"
#include "Transform.h"
#include "Utilities.h"

Level::Level()
	:	levelWidth(0),
		levelDepth(0),
		levelHeight(0),
		levelGrid(0, 0, 0)
{
}

Level::~Level() 
{
	//We trust the shared_ptr to manage any deletions, so no action required here
}

void Level::buildTestLevel()
{
	levelWidth = 20;
	levelHeight = 1;
	levelDepth = 20;
	levelGrid = LevelGrid(levelWidth, levelHeight, levelDepth);
	buildCoreObjects();
	TEST_addLights();	//Add some objects to represent the point lights for debugging purposes
	//Add objects
	Mesh* cube;
	Material* defaultMaterial;

	GraphicsResourceManager& resourceManager = GraphicsResourceManager::getInstance();
	defaultMaterial = resourceManager.loadMaterial("DefaultLit");
	cube = resourceManager.loadMesh("unit_cube");

	//build the contents of the grid
	for (int x = 0; x < levelWidth; x++)
	{
		for (int y = 0; y < levelHeight; y++)
		{
			for (int z = 0; z < levelDepth; z++)
			{
				if (z == 10 && x % 8 != 0)
				{
					TEST_addSolidWall(x, y, z, cube, defaultMaterial);
				}
				else
				{
					TEST_addFloorTile(x, y, z, cube, defaultMaterial);
				}
			}
		}
	}
	//Build the non-grid associated objects
	TEST_addPlayerUnit({ 0, 0, 0 });
	TEST_addPlayerUnit({ 0, 0, 1 });
	TEST_addEnemyUnit({ 3, 0, 3 });
	TEST_addEnemyUnit({ 3, 0, 5 });
	TEST_addEnemyUnit({ 3, 0, 7 });
	
}

void Level::TEST_addFloorTile(int x, int y, int z, Mesh* floorMesh, Material* floorMat)
{
	//Generate the mesh
	std::shared_ptr<StaticMesh> levelBaseTile = std::make_shared<StaticMesh>(floorMesh, floorMat);
	levelBaseTile->name = std::format("Level Floor ({}, {}, {})", x, y, z);
	levelBaseTile->collider = std::make_unique<BoxCollider>();
	levelBaseTile->collider->layer = Collision::Layer_LevelGeometry;
	levelBaseTile->transform = Transform(glm::vec3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)), glm::identity<glm::mat4>(), glm::vec3(1.0f, 0.1f, 1.0f));
	//Create the grid info
	LevelGridCellInfo info
	{
		true,	//walkable
		false,	//occupied
		levelBaseTile.get(),
		nullptr
	};
	levelGrid.addCell(info);
	//add the mesh to the list of things to place within the scene
	objects.push_back(levelBaseTile);
}

void Level::TEST_addSolidWall(int x, int y, int z, Mesh* wallMesh, Material* wallMat)
{
	//Generate the mesh
	std::shared_ptr<StaticMesh> levelWall = std::make_shared<StaticMesh>(wallMesh, wallMat);
	levelWall->name = std::format("Level Wall ({}, {}, {})", x, y, z);
	levelWall->collider = std::make_unique<BoxCollider>();
	levelWall->collider->layer = Collision::Layer_LevelGeometry;
	levelWall->transform = Transform(glm::vec3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)), glm::identity<glm::mat4>(), glm::vec3(1.0f, 1.0f, 1.0f));
	//Create the grid info
	LevelGridCellInfo info
	{
		false,	//walkable
		false,	//occupied
		levelWall.get(),
		nullptr
	};
	levelGrid.addCell(info);
	//add the mesh to the list of things to place within the scene
	objects.push_back(levelWall);
}

void Level::TEST_addPlayerUnit(glm::ivec3 coords)
{
	if (!levelGrid.validCell(coords))
	{
		//Uh oh, trying to place the unit outside of the cell
		coords = { 0, 0, 0 };	//Just default to origin
	}
	//Create a unit for the player to control
	std::shared_ptr<PlayerUnit> playerUnit = std::make_shared<PlayerUnit>(&levelGrid);
	glm::vec3 position = glm::vec3(coords) + Unit::CELL_OFFSET;
	playerUnit->transform = Transform(position, glm::identity<glm::mat4>(), glm::vec3(1.0f, 1.0f, 1.0f));
	objects.push_back(playerUnit);
	int playerLoc = levelGrid.getCellIndexFromSpatialCoords(playerUnit->transform.getPosition());
	levelGrid.setOccupiedState(playerLoc, true);
	playerUnits.push_back(playerUnit);
}

void Level::TEST_addEnemyUnit(glm::ivec3 coords)
{
	if (!levelGrid.validCell(coords))
	{
		//Uh oh, trying to place the unit outside of the cell
		coords = { 0, 0, 0 };	//Just default to origin
	}
	//Create an enemy unit
	std::shared_ptr<EnemyUnit> enemyUnit = std::make_shared<EnemyUnit>(&levelGrid);
	glm::vec3 position = glm::vec3(coords) + Unit::CELL_OFFSET;
	enemyUnit->transform = Transform(position, glm::identity<glm::mat4>(), glm::vec3(1.0f, 1.0f, 1.0f));
	objects.push_back(enemyUnit);
	int enemyLoc = levelGrid.getCellIndexFromSpatialCoords(enemyUnit->transform.getPosition());
	levelGrid.setOccupiedState(enemyLoc, true);
	enemyUnits.push_back(enemyUnit);
}

void Level::TEST_addLights()
{
	//Add lights
	sun.ambient = glm::vec4(0.25f, 0.25f, 0.25f, 0.0f);
	sun.diffuse = glm::vec4(0.7f, 0.7f, 0.7f, 0.0f);
	sun.specular = glm::vec4(0.8f, 0.4f, 0.4f, 0.0f);
	sun.direction = glm::vec4(1.0f, -1.0f, 1.0f, 0.0f);
	for (int i = 0; i < 4; i++)
	{
		PointLight l{};
		l.position = glm::vec4(0.0f, 1.0f, 3.0f * i, 1.0f);
		l.ambient = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		l.diffuse = glm::vec4(0.25f, 0.25f, 0.25f, 0.0f);
		l.specular = glm::vec4(0.1f, 0.1f, 0.5f, 0.0f);
		l.attenuation = glm::vec4(1.0f, 0.09f, 0.032f, 0.0f);
		lights.push_back(l);
		//add a display marker for it
		std::shared_ptr<LightMarker> marker = std::make_shared<LightMarker>(l);
		objects.push_back(marker);
	}
}

void Level::loadLevel(std::string levelName)
{
	DEBUG_PRINT("Error: Loading level from file is not yet implemented\n");
}

void Level::buildCoreObjects()
{
	//Create a line renderer for unit paths
	std::shared_ptr<LineRenderer> pathIndicator = std::make_shared<LineRenderer>(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	std::vector<glm::vec3> linePoints = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, -1.0f)
	};
	pathIndicator->generateSegmentsFromPoints(linePoints);
	pathIndicator->name = "Path Indicator";
	objects.push_back(pathIndicator);

	//Create a planar cursor
	GraphicsResourceManager& resourceManager = GraphicsResourceManager::getInstance();
	std::shared_ptr pathCursor = std::make_shared<StaticMesh>(resourceManager.loadMesh("unit_plane"), resourceManager.loadMaterial("TargetingIndicator"));
	pathCursor->name = "Path Cursor";
	pathCursor->transform.setPosition(glm::vec3(- 0.2f, 0.01f, -0.2f));
	pathCursor->transform.setScale(glm::vec3(0.4f, 1.0f, 0.4f));
	pathCursor->enabled = false;
	pathCursor->castsShadows = false;
	objects.push_back(pathCursor);
}

std::vector<std::shared_ptr<PlayerUnit>> Level::getActivePlayers()
{
	std::vector<std::shared_ptr<PlayerUnit>> activePlayers;
	for (size_t i = 0; i < playerUnits.size(); i++)
	{
		if (playerUnits[i]->enabled && !playerUnits[i]->flaggedForDeletion)
		{
			activePlayers.push_back(playerUnits[i]);
		}
	}
	return activePlayers;
}

std::vector<std::shared_ptr<EnemyUnit>> Level::getActiveEnemies()
{
	std::vector<std::shared_ptr<EnemyUnit>> activeEnemies;
	for (size_t i = 0; i < enemyUnits.size(); i++)
	{
		if (enemyUnits[i]->enabled && !enemyUnits[i]->flaggedForDeletion)
		{
			activeEnemies.push_back(enemyUnits[i]);
		}
	}
	return activeEnemies;
}
