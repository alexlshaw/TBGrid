#include "Level.h"

Level::Level(GraphicsResourceManager* resourceManager) 
	:	resourceManager(resourceManager),
		levelWidth(0),
		levelDepth(0),
		levelHeight(0)
{}

Level::~Level() 
{
	//We trust the shared_ptr to manage any deletions, so no action required here
}

void Level::buildTestLevel()
{
	levelWidth = 20;
	levelHeight = 1;
	levelDepth = 20;
	levelGrid.reserve(static_cast<size_t>(levelWidth * levelHeight * levelDepth));
	buildCoreObjects();
	//Add lights
	lights.push_back(Light(glm::vec3(0.25f, 0.25f, 0.25f), glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(), glm::vec3(-1000.0f, 1000.0f, -1000.0f)));
	//Add objects
	Mesh* cube;
	Material* defaultMaterial;

	defaultMaterial = resourceManager->loadMaterial("DefaultLit");
	cube = resourceManager->loadMesh("unit_cube");

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
	//Create a unit for the player to control
	std::shared_ptr<PlayerUnit> playerUnit = std::make_shared<PlayerUnit>(resourceManager);
	playerUnit->transform = Transform(glm::vec3(0.5f, 0.1f, 0.5f), glm::identity<glm::mat4>(), glm::vec3(1.0f, 1.0f, 1.0f));
	objects.push_back(playerUnit);
}

void Level::TEST_addFloorTile(int x, int y, int z, Mesh* floorMesh, Material* floorMat)
{
	//Generate the mesh
	std::shared_ptr<StaticMesh> levelBaseTile = std::make_shared<StaticMesh>(floorMesh, floorMat);
	levelBaseTile->name = "Level Floor";
	levelBaseTile->collider = std::make_unique<BoxCollider>();
	levelBaseTile->transform = Transform(glm::vec3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)), glm::identity<glm::mat4>(), glm::vec3(1.0f, 0.1f, 1.0f));
	//Create the grid info
	LevelGridCellInfo info
	{
		true,
		levelBaseTile.get(),
		nullptr
	};
	levelGrid.push_back(info);
	//add the mesh to the list of things to place within the scene
	objects.push_back(levelBaseTile);
}

void Level::TEST_addSolidWall(int x, int y, int z, Mesh* wallMesh, Material* wallMat)
{
	//Generate the mesh
	std::shared_ptr<StaticMesh> levelBaseTile = std::make_shared<StaticMesh>(wallMesh, wallMat);
	levelBaseTile->name = "Level Wall";
	levelBaseTile->collider = std::make_unique<BoxCollider>();
	levelBaseTile->transform = Transform(glm::vec3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)), glm::identity<glm::mat4>(), glm::vec3(1.0f, 1.0f, 1.0f));
	//Create the grid info
	LevelGridCellInfo info
	{
		false,
		levelBaseTile.get(),
		nullptr
	};
	levelGrid.push_back(info);
	//add the mesh to the list of things to place within the scene
	objects.push_back(levelBaseTile);
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

int Level::getCellIndexFromUnitCoords(const int x, const int y, const int z) const
{
	return arrayIndexFromCoords3D({ x, y, z }, levelWidth, levelHeight, levelDepth);
}

int Level::getCellIndexFromSpatialCoords(const glm::vec3 location) const
{
	//Because our cells are 1 unit wide, we can just truncate each axis value
	int x = static_cast<int>(location.x);
	int y = static_cast<int>(location.y);
	int z = static_cast<int>(location.z);
	return getCellIndexFromUnitCoords(x, y, z);
}

ArrayCoords3D Level::getUnitCoordsFromCellIndex(const int cellIndex) const
{
	return arrayCoordsFromIndex3D(cellIndex, levelWidth, levelHeight, levelDepth);
}

glm::vec3 Level::getSpatialCoordsFromCellIndex(const int cellIndex) const
{
	ArrayCoords3D coords = getUnitCoordsFromCellIndex(cellIndex);
	return glm::vec3(static_cast<float>(coords.x), static_cast<float>(coords.y), static_cast<float>(coords.z));
}

std::vector<int> Level::pathBetweenTwoCells(int start, int end)
{
	std::vector<int> path;
	//Quick sanity check
	if (levelGrid[start].walkable && levelGrid[end].walkable)
	{
		ArrayCoords3D startCoords = getUnitCoordsFromCellIndex(start);
		ArrayCoords3D endCoords = getUnitCoordsFromCellIndex(end);
		std::vector<AI::Node> nodePath = AI::aStar(levelGrid, AI::Node(startCoords.x, startCoords.y, startCoords.z), AI::Node(endCoords.x, endCoords.y, endCoords.z), levelWidth, levelHeight, levelDepth);
		if (nodePath.size() == 0)
		{
			DEBUG_PRINTLN(std::format("Failed to generate a nav path from ({}, {}, {}) to ({}, {}, {}): no route found.", startCoords.x, startCoords.y, startCoords.z, endCoords.x, endCoords.y, endCoords.z));
		}
		//Convert nodes to indices
		for (size_t i = 0; i < nodePath.size(); i++)
		{
			//DEBUG_PRINTLN(std::format("Nav path adding node: ({}, {}, {})", nodePath[i].x, nodePath[i].y, nodePath[i].z));
			int idx = getCellIndexFromUnitCoords(nodePath[i].x, nodePath[i].y, nodePath[i].z);
			path.push_back(idx);
		}
	}
	else
	{
		DEBUG_PRINTLN("Attempting to generate nav path to/from impossible location");
	}
	return path;
}